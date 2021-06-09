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
#include "message.hpp"
#include "tuple.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"

namespace linda {

class Client {
public:
    Client() {
        LOG_S(INFO) << "Starting client...\n";
    }
    
    void connect();

    std::string read_path;
    std::string write_path;
};

}  // namespace linda
