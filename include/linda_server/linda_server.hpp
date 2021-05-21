#pragma once
#include <fcntl.h>
#include <semaphore.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "linda_spec.hpp"
#include "uuid_maker/uuid_maker.hpp"

namespace linda {

class LindaServer {
public:
    LindaServer();
    ~LindaServer();
    void mainLoop();
private:
    std::string genUuid();
    int32_t fifoHandle;
    sem_t* busSem;
    sem_t* serverSem;
};
}  // namespace linda
