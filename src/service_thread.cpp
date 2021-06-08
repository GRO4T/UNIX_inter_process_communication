#include "service_thread.hpp"
#include "common.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "linda_common.hpp"
#include "serverDB.hpp"

using namespace linda;


linda::ServiceThread::ServiceThread(ServiceThreadParameters params): message_buffer(100) {
    fifo_read = openFIFO(params.paths.read_path, O_RDWR);
    fifo_write = openFIFO(params.paths.write_path, O_RDWR);

    memset(&pfd[0], 0, sizeof(pfd));
    pfd[0].fd = fifo_read;
    pfd[0].events = POLLIN;
    memset(&pfd[1], 0, sizeof(pfd));
    pfd[1].fd = fifo_write;
    pfd[1].events = POLLOUT;

    database = params.databasePtr;

    curr_operation_type = 0;
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
    std::vector<Pattern> pattern_tuple;
    int i = 0;
    while (i < tuple_length){
        auto msg = getMessageOrWait();
        if (msg->GetType() != TYPE_TUPLE_PATTERN_ELEM)
            throw std::runtime_error("Expected tuple pattern elem. Got something else\n");
        DLOG_S(INFO) << "Service thread received tuple pattern elem\n";
        auto pattern_msg = static_cast<Pattern*>(msg.get());
        pattern_tuple.push_back(*pattern_msg);
        i++;
    }
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
    std::vector<Pattern> pattern_tuple;
    int i = 0;
    while (i < tuple_length){
        auto msg = getMessageOrWait();
        if (msg->GetType() != TYPE_TUPLE_PATTERN_ELEM)
            throw std::runtime_error("Expected tuple pattern elem. Got something else\n");
        DLOG_S(INFO) << "Service thread received tuple pattern elem\n";
        auto pattern_msg = static_cast<Pattern*>(msg.get());
        pattern_tuple.push_back(*pattern_msg);
        i++;
    }
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
    std::vector<TupleElem> tuple;
    int i = 0;
    while (i < tuple_length) {
        auto msg = getMessageOrWait();
        if (msg->GetType() != TYPE_TUPLE_ELEM)
            throw std::runtime_error("Expected tuple elem. Got something else\n");
        DLOG_S(INFO) << "Service thread received tuple elem\n";
        auto elem_msg = static_cast<TupleElemMessage*>(msg.get());
        tuple.push_back(elem_msg->elem);
        i++;
    }
    database->addTupleToDB(tuple);
    linda::OperationMessage msg(linda::OP_RETURN_RESULT, 0);
    sendMessage(msg, fifo_write);

}

void linda::ServiceThread::handleOperationMessage(OperationMessage* op_msg){
    curr_operation_type = op_msg->op_type;
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

std::unique_ptr<linda::Message> linda::ServiceThread::getMessageOrWait(){
    std::optional<std::unique_ptr<Message>> msg_optional;
    if ((msg_optional = fetchMessageFromBuffer(message_buffer)) &&
        msg_optional.has_value()) {
        return std::move(msg_optional.value());
    }
    int ret = poll(pfd, 1, -1); //patrze tylko na pierwszy deskryptor, nie wiem czy to nie jakas herezja
    if(ret > 0 && pfd[0].revents & POLLIN){
        bufferedReadFromPipe(message_buffer, fifo_read);
        if ((msg_optional = fetchMessageFromBuffer(message_buffer)) &&
            msg_optional.has_value()) {
            return std::move(msg_optional.value());
        }
    }
}

void* linda::ServiceThread::mainLoop(void* arg){
    auto params = *static_cast<ServiceThreadParameters*>(arg);
    ServiceThread service(params);
    std::optional<std::unique_ptr<Message>> msg_optional;
    std::unique_ptr<Message> recv_msg;

    LOG_S(INFO) << "THIS IS SERVICE THREAD\n";

    bool client_connected = true;
    bool response_prepared = false;
    while(client_connected){
        int ret = poll(service.pfd, 2, -1);
        if(ret > 0 && service.pfd[0].revents & POLLIN){
            LOG_S(INFO) << "Something przyszlo...\n";
            bufferedReadFromPipe(service.message_buffer, service.fifo_read);
            while ((msg_optional = fetchMessageFromBuffer(service.message_buffer)) &&
                    msg_optional.has_value()) {
                recv_msg = std::move(msg_optional.value());
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

        } 
        else if (ret > 0 && service.pfd[1].revents & POLLOUT && response_prepared) {
            LOG_S(INFO) << "hmmmm..\n";
        }
    }

    LOG_S(INFO) << "Ended service work...\n";
    pthread_exit(nullptr);
}