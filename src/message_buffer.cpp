#include "message_buffer.hpp"

#include "message.hpp"

namespace linda {

MessageBuffer::MessageBuffer(uint max_size) : max_size_(max_size), msg_buffer_("") {
}

bool MessageBuffer::push(const std::string& bytes) {
    std::lock_guard lock(m_);
    if (queue_.size() < max_size_) {
        auto it = bytes.begin();
        while (it != bytes.end()) {
            if (*it == MESSAGE_TERM_CHAR)
                queue_.push(msg_buffer_);
            else
                msg_buffer_ += *it;
            ++it;
        }
        if (queue_.size() == 1) {
            cond_reader_.notify_one();
        }
        return true;
    }
    return false;
}

std::optional<std::string> MessageBuffer::popMessage() {
    std::unique_lock lock(m_);
    if (queue_.empty()) {
//        cond_reader_.wait(lock, [this] { return !queue_.empty(); });
        return std::nullopt;
    }
    auto msg = queue_.front();
    queue_.pop();
    return std::optional<std::string>(msg);
}

}
