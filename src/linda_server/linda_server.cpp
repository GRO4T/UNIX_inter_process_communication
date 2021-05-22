#include "linda_server/linda_server.hpp"

linda::LindaServer::LindaServer() {
    // CREATING MUTEX;
    busSem = sem_open(linda::consts::bus_mutex, O_CREAT, linda::consts::perms,
                      linda::consts::mutex_value);
    // CREATING FIFOS;
    int result = mkfifo(linda::consts::linda_bus_read, linda::consts::perms);
    if (result < 0) perror("Error while creating FIFO");
    result = mkfifo(linda::consts::linda_bus_write, linda::consts::perms);
    if (result < 0) perror("Error while creating FIFO");
    fifoRead = open(linda::consts::linda_bus_write, O_RDWR);
    fifoWrite = open(linda::consts::linda_bus_read, O_RDWR);
}

linda::LindaServer::~LindaServer() {
    sem_close(busSem);
    sem_unlink(linda::consts::bus_mutex);
    unlink(linda::consts::linda_bus_write);
    unlink(linda::consts::linda_bus_read);
}

std::string linda::LindaServer::genUuid() {
    auto uuid = UuidMaker::Instance().GenUuid();
    std::string uuidStr = uuids::to_string(uuid);
    return uuidStr;
}

template <class T>
void linda::LindaServer::send(T message, size_t size) const {
    int ret = write(fifoWrite, &message, size);
    if (ret < 0) {
        throw "Error: Could not send through FIFO.";
        return;
    }
}

void linda::LindaServer::sendPaths() {
    std::string uuidStr = genUuid();
    std::string client_write =
        std::string("/tmp/") + uuidStr + std::string("_write.fifo");
    std::string client_read =
        std::string("/tmp/") + uuidStr + std::string("_read.fifo");
    linda::LindaFifoPaths paths;
    strncpy(paths.read_path, client_read.c_str(), linda::consts::max_path);
    strncpy(paths.write_path, client_write.c_str(), linda::consts::max_path);
    send(paths, sizeof(paths));
}

void linda::LindaServer::mainLoop() {
    struct pollfd pfd[2];
    memset(&pfd[0], 0, sizeof(pfd));
    pfd[0].fd = fifoRead;
    pfd[0].events = POLLIN;
    memset(&pfd[1], 0, sizeof(pfd));
    pfd[1].fd = fifoWrite;
    pfd[1].events = POLLOUT;

    bool connected = false;
    while (true) {
        int ret = poll(pfd, 2, -1);
        if (ret > 0 && pfd[0].revents & POLLIN) {
            linda::LindaMessage msg;
            memset(&msg, 0, linda::consts::message_size);
            int result = read(fifoRead, &msg, linda::consts::message_size);
            if (result > 0 && msg.control_mask == linda::commands::connect) {
                connected = true;
            } else if (result < 0) {
                perror("Error read");
            }
        } else if (ret > 0 && pfd[1].revents & POLLOUT && connected) {
            sendPaths();
            connected = false;
        }
    }
}