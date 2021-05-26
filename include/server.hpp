#pragma once
#include <fcntl.h>
#include <semaphore.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "linda_helpers.hpp"
#include "linda_spec.hpp"
#include "uuid_maker.hpp"

namespace linda {

class LindaServer {
public:
    LindaServer();
    ~LindaServer();
    void mainLoop();

private:
    LindaFifoPaths sendPaths();
    //MUSI BYĆ STATIC BO INACZEJ SEGFAULT
    static void* service(void * paths);
    std::string genUuid();

    int32_t fifo_write;
    int32_t fifo_read;
    sem_t* bus_sem;
    std::vector<pthread_t> service_threads;
};
}  // namespace linda
