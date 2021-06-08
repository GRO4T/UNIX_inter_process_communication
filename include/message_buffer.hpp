#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <optional>

namespace linda {

class MessageBuffer {
public:
    MessageBuffer(uint max_size);

    bool push(const std::string& bytes);
    std::optional<std::string> popMessage();

private:
    std::string msg_buffer_;
    std::queue<std::string> queue_;
    uint max_size_;

    std::mutex m_;
    std::condition_variable cond_reader_;
};


}