#include "client.hpp"

#include "common.hpp"
#include "deserializer.hpp"
#include "linda_common.hpp"
#include "tuple.hpp"

using namespace linda;

void Client::communicate(){

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
    sendTuple(OP_LINDA_WRITE,
              {1},
              fifo_write);
    sendPattern(
        OP_LINDA_READ,
//        {Int("==3"), String("==tak"), Float("==46.03"), String("==nie"), Int("!=1999")},
        {Int("*")},
        fifo_write);

    sleep(3);
    bufferedReadFromPipe(message_buffer, fifo_read);
    while ((msg_optional = fetchMessageFromBuffer(message_buffer)) &&
                    msg_optional.has_value()) {
                recv_msg = std::move(msg_optional.value());
                if(recv_msg->GetType() == linda::TYPE_OPERATION_MSG){
                    LOG_S(INFO) << "Dostalem Potwierdzenie";
                }
    }

    disconnect();

}

void linda::Client::disconnect() {
    ConnectionMessage msg(false);
    sendMessage(msg, fifo_write);
    LOG_S(INFO) << "Disconnected";
}

void linda::Client::connect() {
    // connect to linda bus
    sem_t* bus_sem = sem_open(consts::bus_mutex, 0);
    if (bus_sem == nullptr)
        LOG_S(ERROR) << "Error while opening semaphore\n";
    sem_wait(bus_sem);
    int bus_read = openFIFO(consts::linda_bus_read, O_RDONLY);
    int bus_write = openFIFO(consts::linda_bus_write, O_WRONLY);
    // send connection request
    LOG_S(INFO) << "Client trying to connect...\n";
    ConnectionMessage msg(true);
    sendMessage(msg, bus_write);
    // Receive connection response with FIFO names
    auto bytes = readBytes(bus_read);
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
        fifo_read = linda::openFIFO(read_path, O_RDWR);
        fifo_write = linda::openFIFO(write_path, O_RDWR);
        communicate();
    }
    else {
        throw std::runtime_error("Bad message type in this context. Expected ServerConnectionResponse!");
    }
    // tidy up
    closeFIFO(bus_read);
    closeFIFO(bus_write);
}
