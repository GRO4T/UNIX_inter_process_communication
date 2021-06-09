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

using Command = std::pair<OperationType, std::vector<std::variant<TupleElem, Pattern> > >;

class Client {
public:
    Client() : message_buffer(100) {
        LOG_S(INFO) << "Starting client...\n";
    }
    void interact();
    void connect();
    void disconnect();
    void handleCommand(Command cmd);

    MessageBuffer message_buffer;
    std::string read_path;
    std::string write_path;
    int32_t fifo_read;
    int32_t fifo_write;
};

}  // namespace linda
