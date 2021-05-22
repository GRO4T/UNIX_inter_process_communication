#include "linda_server/linda_server.hpp"

linda::LindaServer::LindaServer() {
    // CREATING MUTEX;
    busSem = sem_open(linda::consts::bus_mutex, O_CREAT, linda::consts::perms,
                      linda::consts::mutex_value);
    // CREATING FIFOS;
    makeFIFO(linda::consts::linda_bus_read, linda::consts::perms);
    makeFIFO(linda::consts::linda_bus_write, linda::consts::perms);

    fifoRead = openFIFO(linda::consts::linda_bus_write, O_RDWR);
    fifoWrite = openFIFO(linda::consts::linda_bus_read, O_RDWR);
}

linda::LindaServer::~LindaServer() {
    sem_close(busSem);
    sem_unlink(linda::consts::bus_mutex);
    closeFIFO(fifoRead);
    closeFIFO(fifoWrite);
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

linda::LindaFifoPaths linda::LindaServer::sendPaths() {
    std::string uuidStr = genUuid();
    std::string client_write =
        std::string("/tmp/") + uuidStr + std::string("_write.fifo");
    std::string client_read =
        std::string("/tmp/") + uuidStr + std::string("_read.fifo");
    linda::LindaFifoPaths paths;
    strncpy(paths.read_path, client_read.c_str(), linda::consts::max_path);
    strncpy(paths.write_path, client_write.c_str(), linda::consts::max_path);

    makeFIFO(paths.write_path, linda::consts::perms);
    makeFIFO(paths.read_path, linda::consts::perms);

    send(paths, sizeof(paths));
    return paths;
}

void* linda::LindaServer::service(void *  paths) {
    std::cout << "THIS IS SERVICE THREAD" << std::endl;
    linda::LindaFifoPaths* castedPaths = static_cast<linda::LindaFifoPaths*>(paths);

    uint32_t fifoRead = openFIFO(castedPaths->write_path, O_RDONLY);
    uint32_t fifoWrite = openFIFO(castedPaths->read_path, O_WRONLY);

    char buffer[linda::consts::max_path];
    memset(&buffer, 0, sizeof(buffer)); 
    read(fifoRead, &buffer, sizeof(buffer));
    std::cout<<buffer<<std::endl;
    char msg[linda::consts::max_path] = "Hello from server!";
    write(fifoWrite, &msg, sizeof(msg));

    sleep(1);
    std::cout << "Work..." << std::endl;

    closeFIFO(fifoRead);
    closeFIFO(fifoWrite);
    unlink(castedPaths->write_path);
    unlink(castedPaths->read_path);
    pthread_exit(nullptr);
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
            linda::LindaFifoPaths paths = sendPaths();
            pthread_t serviceThread;
            pthread_create(&serviceThread, NULL, linda::LindaServer::service, (void*) &paths);
            serviceThreads.push_back(serviceThread);
            connected = false;
        }
    }
}