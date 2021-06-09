#include "service_thread.hpp"

#include "common.hpp"
#include "deserializer.hpp"
#include "linda_common.hpp"
#include "serializer.hpp"
#include "server_db.hpp"

using namespace linda;


linda::ServiceThread::ServiceThread(ServiceThreadParameters params): message_buffer(100) {
    fifo_read = openFIFO(params.paths.read_path, O_RDWR);
    fifo_write = openFIFO(params.paths.write_path, O_RDWR);
    database = params.databasePtr;
}

bool linda::ServiceThread::handleConnectionMessage(Message* msg){
    ConnectionMessage* conn_msg = dynamic_cast<ConnectionMessage*>(msg);

    if( conn_msg->connect ){
        LOG_S(INFO) << "Client trying to connected more than once\n";
        return true;
    }
    else{
        LOG_S(INFO) << "Client wants to disconnect\n";
        return false;
    }
}

void ServiceThread::handleRead(int tuple_length) {
    DLOG_S(INFO) << "Service thread began handling read operation\n";
    auto pattern_tuple = receivePattern(tuple_length, message_buffer, fifo_read);
    auto tuple = database->findTuple(pattern_tuple);
    if (tuple.empty()) {
        database->waitForTuple(pattern_tuple);
        linda::OperationMessage msg(linda::OP_RETURN_RESULT, 0);
        sendMessage(msg, fifo_write);
    }
    else {
        linda::OperationMessage msg(linda::OP_RETURN_RESULT, tuple.size());
        sendMessage(msg, fifo_write);
        DLOG_S(INFO) << "Service thread tuple found\n";
    }
}

void ServiceThread::handleInput(int tuple_length) {
    DLOG_S(INFO) << "Service thread began handling input operation\n";
    auto pattern_tuple = receivePattern(tuple_length, message_buffer, fifo_read);
    auto tuple = database->findTupleAndRemoveIt(pattern_tuple);
    if( tuple.empty() ){
        database->waitForTuple(pattern_tuple);
        linda::OperationMessage msg(linda::OP_RETURN_RESULT, 0);
        sendMessage(msg, fifo_write);
    }
    else{
        linda::OperationMessage msg(linda::OP_RETURN_RESULT, tuple.size());
        sendMessage(msg, fifo_write);
        DLOG_S(INFO) << "Service thread tuple found\n";
    }
}

void ServiceThread::handleWrite(int tuple_length) {
    DLOG_S(INFO) << "Service thread began handling write operation\n";
    auto tuple = receiveTuple(tuple_length, message_buffer, fifo_read);
    database->addTupleToDB(tuple);
    linda::OperationMessage msg(linda::OP_RETURN_RESULT, 0);
    sendMessage(msg, fifo_write);
}

void linda::ServiceThread::handleOperationMessage(OperationMessage* op_msg){
    switch (op_msg->op_type) {
        case OP_LINDA_READ:
            handleRead(op_msg->tuple_size);
            break;
        case OP_LINDA_WRITE:
            handleWrite(op_msg->tuple_size);
            break;
        case OP_LINDA_INPUT:
            handleInput(op_msg->tuple_size);
            break;
    }
}

void* linda::ServiceThread::mainLoop(void* arg){
    auto params = *static_cast<ServiceThreadParameters*>(arg);
    ServiceThread service(params);
    std::unique_ptr<Message> recv_msg;

    LOG_S(INFO) << "THIS IS SERVICE THREAD\n";

    bool client_connected = true;
    while(client_connected){
        recv_msg = readFromPipeUntilMessageFound(service.message_buffer, service.fifo_read);
        switch ( recv_msg->GetType() ){
            case linda::TYPE_CONNECTION_MSG:
                client_connected = service.handleConnectionMessage(recv_msg.get());
                break;
            case linda::TYPE_OPERATION_MSG:
                service.handleOperationMessage(static_cast<OperationMessage*>(recv_msg.get()));
                break;
            default:
                LOG_S(INFO) << "Wut?\n";
                break;
        }
    }
    LOG_S(INFO) << "Ended service work...\n";
    pthread_exit(nullptr);
}