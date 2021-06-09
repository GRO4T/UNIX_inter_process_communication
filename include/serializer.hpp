#pragma once

#include <string>
#include <stdexcept>
#include "message.hpp"
#include "tuple.hpp"

namespace linda {

template <class T>
void serialize(std::back_insert_iterator<std::string> buffer, T value);

void serialize(std::back_insert_iterator<std::string> buffer,
               const std::string& value);

std::string serialize(ConnectionMessage& msg);

std::string serialize(ServerConnectionResponse& msg);

std::string serialize(OperationMessage& msg);
std::string serialize(TupleElemMessage& msg);
std::string serialize(Pattern& msg);

}  // namespace linda
