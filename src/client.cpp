#include "client.hpp"

#include "common.hpp"
#include "deserializer.hpp"
#include "linda_common.hpp"
#include "tuple.hpp"

using namespace linda;

void Client::sendTuples(std::string readPath, std::string writePath){
    int32_t fifo_read = linda::openFIFO(readPath, O_RDWR);
    int32_t fifo_write = linda::openFIFO(writePath, O_RDWR);

    /*sleep(3);
    linda::OperationMessage msg(linda::OP_LINDA_WRITE, 5);
    sendMessage(msg, fifo_write);

    linda::TupleElemMessage msg2(3);
    sendMessage(msg2, fifo_write);

    linda::TupleElemMessage msg3("tak");
    linda::sendMessage(msg3, fifo_write);

    linda::TupleElemMessage msg8(46.03);
    linda::sendMessage(msg8, fifo_write);

    linda::TupleElemMessage msg9("nie");
    linda::sendMessage(msg9, fifo_write);

    linda::TupleElemMessage msg10(2000);
    linda::sendMessage(msg10, fifo_write);

    sleep(3);*/
    std::optional<std::unique_ptr<Message>> msg_optional;
    std::unique_ptr<Message> recv_msg;/*
    bufferedReadFromPipe(message_buffer, fifo_read);
    while ((msg_optional = fetchMessageFromBuffer(message_buffer)) &&
                    msg_optional.has_value()) {
                recv_msg = std::move(msg_optional.value());
                if(recv_msg->GetType() == linda::TYPE_OPERATION_MSG){
                    auto msg = static_cast<OperationMessage*>(recv_msg.get());
                    LOG_S(INFO) << "Dostalem Potwierdzenie";
                }
            }
    */
    sleep(3);
    linda::OperationMessage msg4(linda::OP_LINDA_READ, 5);
    linda::sendMessage(msg4, fifo_write);

    linda::Int msg5("==3");
    linda::sendMessage(msg5, fifo_write);

    linda::String msg6("==tak");
    linda::sendMessage(msg6, fifo_write);

    linda::Float msg11("==46.03");
    linda::sendMessage(msg11, fifo_write);

    linda::String msg12("==nie");
    linda::sendMessage(msg12, fifo_write);

    linda::Int msg13("!=1999");
    linda::sendMessage(msg13, fifo_write);

    sleep(3);
    bufferedReadFromPipe(message_buffer, fifo_read);
    while ((msg_optional = fetchMessageFromBuffer(message_buffer)) &&
                    msg_optional.has_value()) {
                recv_msg = std::move(msg_optional.value());
                if(recv_msg->GetType() == linda::TYPE_OPERATION_MSG){
                    auto msg = static_cast<OperationMessage*>(recv_msg.get());
                    LOG_S(INFO) << "Dostalem Potwierdzenie";
                }
            }

    linda::ConnectionMessage msg7(false);
    linda::sendMessage(msg7, fifo_write);

}

void linda::Client::connect() {
    // connect to linda bus
    sem_t* bus_sem = sem_open(consts::bus_mutex, 0);
    if (bus_sem == nullptr)
        LOG_S(ERROR) << "Error while opening semaphore\n";
    sem_wait(bus_sem);
    int fifo_read = openFIFO(consts::linda_bus_read, O_RDONLY);
    int fifo_write = openFIFO(consts::linda_bus_write, O_WRONLY);
    // send connection request
    LOG_S(INFO) << "Client trying to connect...\n";
    ConnectionMessage msg(true);
    sendMessage(msg, fifo_write);
    // Receive connection response with FIFO names
    auto bytes = readBytes(fifo_read);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    if (recv_msg->GetType() == TYPE_SERVER_CONN_RESPONSE) {
        auto recv_conn_resp_msg = static_cast<ServerConnectionResponse*>(recv_msg.get());
        if (!recv_conn_resp_msg->connected)
            throw std::runtime_error("Client could not connect!");
        LOG_S(INFO) << "Client connected...\n";
        read_path = recv_conn_resp_msg->fifo_read;
        write_path = recv_conn_resp_msg->fifo_write;
        DLOG_S(INFO) << "FIFO READ: " << read_path << std::endl;
        DLOG_S(INFO) << "FIFO WRITE: " << write_path << std::endl;

        sem_post(bus_sem);
        sem_close(bus_sem);
        sendTuples(read_path, write_path);
    }
    else {
        throw std::runtime_error("Bad message type in this context. Expected ServerConnectionResponse!");
    }
    // tidy up
    closeFIFO(fifo_read);
    closeFIFO(fifo_write);
}
