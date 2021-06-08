#pragma once
#include <memory>
#include "linda_common.hpp"
#include "message.hpp"

namespace linda{

class ServiceThread{
public:
    ServiceThread(FifoPaths paths);
    static void* mainLoop(void* arg);

private:
    bool handleConnectionMessage(Message* msg);
    void handleOperationMessage(Message* msg);
    std::unique_ptr<Message> getMessageOrWait();

    int32_t fifo_read;
    int32_t fifo_write;
    struct pollfd pfd[2];

    MessageBuffer message_buffer;

    int awaited_tuple_segments;
    OperationType curr_operation_type;
};

} //namespace linda