#include "linda_server/linda_server.hpp"

linda::LindaServer::LindaServer() {
    // CREATING MUTEX;
    busSem = sem_open(linda::consts::bus_mutex, O_CREAT,
                        linda::consts::perms, linda::consts::mutex_value);
    // serverSem = sem_open(linda::consts::inter_server_mutex, O_CREAT,
    //                     linda::consts::perms, linda::consts::mutex_value);
    // CREATING FIFO;
    int result = mkfifo(linda::consts::linda_bus, linda::consts::perms);
    if (result < 0) perror("Error while creating FIFO");
    fifoHandle = open(linda::consts::linda_bus, O_RDWR);
}

linda::LindaServer::~LindaServer() {
    sem_close(busSem);
    sem_close(serverSem);
    sem_unlink(linda::consts::bus_mutex);
    sem_unlink(linda::consts::inter_server_mutex);
    unlink(linda::consts::linda_bus);
}

std::string linda::LindaServer::genUuid() {
    auto uuid = UuidMaker::Instance().GenUuid();
    std::string uuidStr = uuids::to_string(uuid);
    return uuidStr;
}

void linda::LindaServer::mainLoop() {
    struct pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = fifoHandle;
    pfd.events = POLLIN | POLLOUT;
    bool connected = false;
    while (true) {
        int ret = poll(&pfd, 1, -1);
        if (ret > 0 && pfd.revents & POLLIN) {
            linda::LindaMessage msg;
            memset(&msg, 0, linda::consts::message_size);
            int result = read(fifoHandle, &msg, linda::consts::message_size);
            if (result > 0 && msg.control_mask == linda::commands::connect) {
                connected = true;
            } else if (result < 0) {
                perror("Error read");
            }
        } else if (ret > 0 && pfd.revents & POLLOUT && connected) {
            std::cout << "CLIENT WANT TO CONNEC" << std::endl;
            std::string uuidStr = genUuid();
            std::string client_write =
                std::string("/tmp/") + uuidStr + std::string("_write.fifo");
            std::string client_read =
                std::string("/tmp/") + uuidStr + std::string("_read.fifo");
            linda::LindaFifoPaths paths;
            std::cout << client_write << std::endl;
            std::cout << client_read << std::endl;
            strncpy(paths.read_path, client_read.c_str(),
                    linda::consts::max_path);
            strncpy(paths.write_path, client_write.c_str(),
                    linda::consts::max_path);
            // sem_wait(serverSem);
            write(fifoHandle, &paths, sizeof(paths));
            connected = false;
        }
    }
}