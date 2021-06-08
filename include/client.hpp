#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "linda_common.hpp"
#include "common.hpp"

namespace linda {

class Client {
public:
    Client() : message_buffer(100) {
        LOG_S(INFO) << "Starting client...\n";
    }
    void sendTuples(std::string readPath, std::string writePath);
    void connect();

    MessageBuffer message_buffer;
    std::string read_path;
    std::string write_path;

};

}  // namespace linda
