#pragma once
#include <fcntl.h>
#include <semaphore.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "linda_common.hpp"
#include "message.hpp"
#include "message_buffer.hpp"
#include "serverDB.hpp"

namespace linda {

class Server {
public:
    Server();
    ~Server();
    void mainLoop();

private:
    FifoPaths sendPaths();
    std::string genUuid();

    MessageBuffer message_buffer;

    int32_t fifo_write;
    int32_t fifo_read;
    ServerDB database;
    sem_t* bus_sem;
    std::vector<pthread_t> service_threads;
};
}  // namespace linda
