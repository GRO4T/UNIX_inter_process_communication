#include "server.hpp"

#include "message.hpp"
#include "common.hpp"
#include "deserializer.hpp"
#include "linda_common.hpp"
#include "serializer.hpp"
#include "service_thread.hpp"

linda::Server::Server(): message_buffer(100) {
    LOG_S(INFO) << "Starting server...\n";
    // CREATING MUTEX;
    bus_sem = sem_open(linda::consts::bus_mutex, O_CREAT, linda::consts::perms,
                      linda::consts::mutex_value);
    // CREATING FIFOS;
    makeFIFO(linda::consts::linda_bus_read, linda::consts::perms);
    makeFIFO(linda::consts::linda_bus_write, linda::consts::perms);

    fifo_read = openFIFO(linda::consts::linda_bus_write, O_RDWR);
    fifo_write = openFIFO(linda::consts::linda_bus_read, O_RDWR);
}

linda::Server::~Server() {
    sem_close(bus_sem);
    sem_unlink(linda::consts::bus_mutex);
    closeFIFO(fifo_read);
    closeFIFO(fifo_write);
    unlink(linda::consts::linda_bus_write);
    unlink(linda::consts::linda_bus_read);
}

std::string linda::Server::genUuid() {
    auto uuid = UuidMaker::Instance().GenUuid();
    std::string uuidStr = uuids::to_string(uuid);
    return uuidStr;
}

linda::FifoPaths linda::Server::sendPaths() {
    std::string uuidStr = genUuid();
    std::string client_write =
        std::string("/tmp/") + uuidStr + std::string("_write.fifo");
    std::string client_read =
        std::string("/tmp/") + uuidStr + std::string("_read.fifo");
    makeFIFO(client_write, linda::consts::perms);
    makeFIFO(client_read, linda::consts::perms);

    ServerConnectionResponse msg(true, client_write, client_read);
    sendBytes(serialize(msg), fifo_write);
    return {client_write, client_read};
}

void linda::Server::mainLoop() {
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
            bufferedReadFromPipe(message_buffer, fifo_read);
            std::optional<std::unique_ptr<Message>> msg_optional;
            while ((msg_optional = fetchMessageFromBuffer(message_buffer)) &&
                   msg_optional.has_value()) {
                auto recv_msg = std::move(msg_optional.value());
                if (recv_msg->GetType() == TYPE_CONNECTION_MSG) {
                    LOG_S(INFO) << "Client trying to connect...\n";
                    auto recv_conn_resp_msg = static_cast<ConnectionMessage*>(recv_msg.get());
                    if (recv_conn_resp_msg->connect) {
                        connected = true;
                    }
                }
                else {
                    throw std::runtime_error("Bad message type in this context. Expected ConnectionMessage!");
                }
            }
        } else if (ret > 0 && pfd[1].revents & POLLOUT && connected) {
            LOG_S(INFO) << "Server approved client's connection...\nReturning response...\n";
            ServiceThreadParameters params(sendPaths(), &database);
            pthread_t thread;
            pthread_create(&thread, NULL, linda::ServiceThread::mainLoop, (void*) &params);
            service_threads.push_back(thread);
            connected = false;
        }
    }
}