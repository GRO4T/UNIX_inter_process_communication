#include "service_thread.hpp"
#include "common.hpp"
#include "message.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"

linda::ServiceThread::ServiceThread(FifoPaths paths){
    fifo_read = openFIFO(paths.read_path, O_RDWR);
    fifo_write = openFIFO(paths.write_path, O_RDWR);

    memset(&pfd[0], 0, sizeof(pfd));
    pfd[0].fd = fifo_read;
    pfd[0].events = POLLIN;
    memset(&pfd[1], 0, sizeof(pfd));
    pfd[1].fd = fifo_write;
    pfd[1].events = POLLOUT;

    awaited_tuple_segments = 0;
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

void linda::ServiceThread::handleOperationMessage(Message* msg){
    OperationMessage* opp_msg = dynamic_cast<OperationMessage*>(msg);
    curr_operation_type = opp_msg->op_type;
    awaited_tuple_segments = opp_msg->tuple_size;

    std::vector<std::unique_ptr<Message>> msg_vec;
    std::vector<TupleElem> tuple;
    std::vector<Pattern> pattern_tuple;
    for(int i = 0; i < awaited_tuple_segments; ++i){

        if(curr_operation_type == OP_LINDA_WRITE){
            auto msg = getMessageOrWait();
            auto elem_msg = dynamic_cast<TupleElemMessage*>(msg.get());

            tuple.push_back(elem_msg->elem);
            //addTupleToDB(tuple);
        }

        if(curr_operation_type == OP_LINDA_READ){
            auto msg = getMessageOrWait();
            auto pattern_msg = dynamic_cast<Pattern*>(msg.get());

            pattern_tuple.push_back(*pattern_msg);
            //findTuple(pattern_tuple);
        }

        if(curr_operation_type == OP_LINDA_INPUT){
            auto msg = getMessageOrWait();
            auto pattern_msg = dynamic_cast<Pattern*>(msg.get());

            pattern_tuple.push_back(*pattern_msg);
            //findTupleAndRemoveIt(pattern_tuple);
        }
    }
}

std::unique_ptr<linda::Message> linda::ServiceThread::getMessageOrWait(){
    std::unique_ptr<Message> recv_msg;
    int ret = poll(pfd, 1, -1); //patrze tylko na pierwszy deskryptor, nie wiem czy to nie jakas herezja
    if(ret > 0 && pfd[0].revents & POLLIN){
        auto bytes = readBytes(fifo_read);
        auto c_it = bytes.cbegin();
        recv_msg = deserialize(c_it, bytes.cend());
    }
    return recv_msg;
}

void* linda::ServiceThread::mainLoop(void* arg){
    auto fifo_paths = *static_cast<FifoPaths*>(arg);
    ServiceThread service(fifo_paths);

    LOG_S(INFO) << "THIS IS SERVICE THREAD\n";

    bool client_connected = true;
    bool response_prepared = false;
    while(client_connected){
        int ret = poll(service.pfd, 2, -1);
        if(ret > 0 && service.pfd[0].revents & POLLIN){
            LOG_S(INFO) << "Something przyszlo...\n";
            auto bytes = readBytes(service.fifo_read);
            auto c_it = bytes.cbegin();
            auto recv_msg = deserialize(c_it, bytes.cend());

            switch ( recv_msg->GetType() ){
                case linda::TYPE_CONNECTION_MSG:
                    client_connected = service.handleConnectionMessage(recv_msg.get());
                    break;
                case linda::TYPE_OPERATION_MSG:
                    service.handleOperationMessage(recv_msg.get());
                    break;
                default:
                    LOG_S(INFO) << "Wut?\n";
                    break;
            }

        } 
        else if (ret > 0 && service.pfd[1].revents & POLLOUT && response_prepared) {
            LOG_S(INFO) << "hmmmm..\n";
        }
    }

    LOG_S(INFO) << "Ended service work...\n";
    pthread_exit(nullptr);
}