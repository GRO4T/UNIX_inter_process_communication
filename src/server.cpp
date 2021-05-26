#include "server.hpp"

#include "common.hpp"
#include "deserializer.hpp"
#include "linda_helpers.hpp"
#include "message.hpp"
#include "serializer.hpp"

linda::LindaServer::LindaServer() {
    // CREATING MUTEX;
    bus_sem = sem_open(linda::consts::bus_mutex, O_CREAT, linda::consts::perms,
                      linda::consts::mutex_value);
    // CREATING FIFOS;
    makeFIFO(linda::consts::linda_bus_read, linda::consts::perms);
    makeFIFO(linda::consts::linda_bus_write, linda::consts::perms);

    fifo_read = openFIFO(linda::consts::linda_bus_write, O_RDWR);
    fifo_write = openFIFO(linda::consts::linda_bus_read, O_RDWR);
}

linda::LindaServer::~LindaServer() {
    sem_close(bus_sem);
    sem_unlink(linda::consts::bus_mutex);
    closeFIFO(fifo_read);
    closeFIFO(fifo_write);
    unlink(linda::consts::linda_bus_write);
    unlink(linda::consts::linda_bus_read);
}

std::string linda::LindaServer::genUuid() {
    auto uuid = UuidMaker::Instance().GenUuid();
    std::string uuidStr = uuids::to_string(uuid);
    return uuidStr;
}

linda::LindaFifoPaths linda::LindaServer::sendPaths() {
    std::string uuidStr = genUuid();
    std::string client_write =
        std::string("/tmp/") + uuidStr + std::string("_write.fifo");
    std::string client_read =
        std::string("/tmp/") + uuidStr + std::string("_read.fifo");
    makeFIFO(client_write, linda::consts::perms);
    makeFIFO(client_read, linda::consts::perms);

    ServerConnectionResponse msg(true, client_write, client_read);
    sendBytes(serialize(msg), fifo_write);
    return LindaFifoPaths{client_write, client_read};
}

void* linda::LindaServer::service(void *  paths) {
    LOG_S(INFO) << "THIS IS SERVICE THREAD\n";

    sleep(1);
    std::cout << "Work..." << std::endl;

    pthread_exit(nullptr);
}

void linda::LindaServer::mainLoop() {
    struct pollfd pfd[2];
    memset(&pfd[0], 0, sizeof(pfd));
    pfd[0].fd = fifo_read;
    pfd[0].events = POLLIN;
    memset(&pfd[1], 0, sizeof(pfd));
    pfd[1].fd = fifo_write;
    pfd[1].events = POLLOUT;

    bool connected = false;
    while (true) {
        int ret = poll(pfd, 2, -1);
        if (ret > 0 && pfd[0].revents & POLLIN) {
            auto bytes = readBytes(fifo_read);
            auto c_it = bytes.cbegin();
            auto recv_msg = deserialize(c_it, bytes.cend());
            if (recv_msg->GetType() == TYPE_CONNECTION_MSG) {
                auto recv_conn_resp_msg = static_cast<ConnectionMessage*>(recv_msg.get());
                if (recv_conn_resp_msg->connect) {
                    connected = true;
                }
            }
            else {
                throw std::runtime_error("Bad message type in this context. Expected ConnectionMessage!");
            }
        } else if (ret > 0 && pfd[1].revents & POLLOUT && connected) {
            linda::LindaFifoPaths paths = sendPaths();
            pthread_t serviceThread;
            pthread_create(&serviceThread, NULL, linda::LindaServer::service, (void*) &paths);
            service_threads.push_back(serviceThread);
            connected = false;
        }
    }
}