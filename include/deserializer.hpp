#ifndef DESERIALIZER_HPP
#define DESERIALIZER_HPP

#include <exception>
#include <memory>

#include "message.hpp"

namespace Lindux
{

using StringConstIt = std::string::const_iterator;

class NoMessageFound : public std::exception {
public:
    const char* what() { return "No message found"; }
};

std::unique_ptr<Message> deserialize(StringConstIt& begin, StringConstIt end);
} // namespace Lindux

#endif