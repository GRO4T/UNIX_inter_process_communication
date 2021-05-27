#pragma once

#include <exception>
#include <memory>

#include "message.hpp"

namespace linda {

using StringConstIt = std::string::const_iterator;

class NoMessageFound : public std::exception {
public:
    const char* what() { return "No message found"; }
};

std::unique_ptr<Message> deserialize(StringConstIt& begin, StringConstIt end);
}  // namespace linda
