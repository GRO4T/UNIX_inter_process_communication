#include "linda_functions.hpp"

#include "common.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"

linda::LindaFifoPaths linda::lindaConnect() {
    // init
    sem_t* bus_sem = sem_open(consts::bus_mutex, 0);
    if (bus_sem == nullptr)
        LOG_S(ERROR) << "Error while opening semaphore\n";
    sem_wait(bus_sem);
    int fifo_read = openFIFO(consts::linda_bus_read, O_RDONLY);
    int fifo_write = openFIFO(consts::linda_bus_write, O_WRONLY);
    LindaFifoPaths paths;
    memset(&paths, 0, sizeof(paths));
    // send connection request
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
        paths.read_path = recv_conn_resp_msg->fifo_read;
        paths.write_path = recv_conn_resp_msg->fifo_write;
    }
    else {
        throw std::runtime_error("Bad message type in this context. Expected ServerConnectionResponse!");
    }
    DLOG_S(INFO) << "READ: " << paths.read_path << std::endl;
    DLOG_S(INFO) << "WRITE: " << paths.write_path << std::endl;
    // tidy up
    closeFIFO(fifo_read);
    closeFIFO(fifo_write);
    sem_post(bus_sem);
    sem_close(bus_sem);
    return paths;
}
