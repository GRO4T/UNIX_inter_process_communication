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
#include "linda_helpers.hpp"
#include "uuid_maker/uuid_maker.hpp"

namespace linda {

class LindaServer {
public:
    LindaServer();
    ~LindaServer();
    void mainLoop();

private:
    template <class T>
    void send(T message, size_t size) const;
    void sendPaths();
    void service(linda::LindaFifoPaths paths);
    std::string genUuid();
    int32_t fifoWrite;
    int32_t fifoRead;
    sem_t* busSem;
};
}  // namespace linda
