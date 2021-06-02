#include "client.hpp"

#include "common.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"

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
    sendBytes(serialize(msg), fifo_write);
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
    }
    else {
        throw std::runtime_error("Bad message type in this context. Expected ServerConnectionResponse!");
    }
    // tidy up
    closeFIFO(fifo_read);
    closeFIFO(fifo_write);
    sem_post(bus_sem);
    sem_close(bus_sem);

    read_desk = open(read_path, O_RDONLY);
    write_desk = open(write_path, O_WRONLY);
}

void linda::Client::disconnect(){
    close(read_desk);
    close(write_desk);
}

std::vector<linda::TupleElem> linda::Client::input(std::vector<linda::Pattern> tuple, int timeout){
    return std::vector<linda::TupleElem>();
}

void linda::Client::output(std::vector<linda::TupleElem> tuple){
    
}

std::vector<linda::TupleElem> linda::Client::read(std::vector<linda::Pattern> typle, int timeout){
    return std::vector<linda::TupleElem>();
}
