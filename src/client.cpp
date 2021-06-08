#include "client.hpp"

#include "common.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "linda_common.hpp"

using namespace linda;

void sendTuples(std::string readPath, std::string writePath){
    int32_t fifo_read = linda::openFIFO(readPath, O_RDWR);
    int32_t fifo_write = linda::openFIFO(writePath, O_RDWR);

    linda::OperationMessage msg(linda::OP_LINDA_WRITE, 2);
    sendMessage(msg, fifo_write);

    linda::TupleElemMessage msg2(3);
    sendMessage(msg2, fifo_write);

    // sleep(1);
    while (true) {
        linda::TupleElemMessage msg3("somestring");
        sendMessage(msg3, fifo_write);
    }

    // sleep(2);
    linda::ConnectionMessage msg4(false);
    linda::sendBytes(linda::serialize(msg4), fifo_write);

    //linda::OperationMessage msg(linda::OP_LINDA_WRITE, 1);
    //linda::sendBytes(linda::serialize(msg), fifo_write);

    //sleep(4);
    //char buffer[linda::consts::max_path];
    //memset(&buffer, 0, sizeof(buffer)); 
    //read(fifo_read, &buffer, sizeof(buffer));
    //LOG_S(INFO)<<buffer<<std::endl;

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

        sendTuples(read_path, write_path);
    }
    else {
        throw std::runtime_error("Bad message type in this context. Expected ServerConnectionResponse!");
    }
    // tidy up
    closeFIFO(fifo_read);
    closeFIFO(fifo_write);
    sem_post(bus_sem);
    sem_close(bus_sem);
}
