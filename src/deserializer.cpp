#include "deserializer.hpp"

#include <cstring>
#include <map>
#include <algorithm>

namespace Lindux
{

template <class T>
bool canDeserialize(StringConstIt begin, StringConstIt end) {
    return (std::size_t)std::distance(begin, end) >= sizeof(T);
}

bool canDeserialize(StringConstIt begin, StringConstIt end, std::size_t bytes) {
    return (std::size_t)std::distance(begin, end) >= bytes;
}

template <class T>
void deserialize(T* value, StringConstIt& begin, StringConstIt end) {
    if (!canDeserialize<T>(begin, end))
        throw std::runtime_error("Cannot correctly deserialize the message. Not enough bytes.");
    std::memcpy(value, &*begin, sizeof(T));
    std::advance(begin, sizeof(T));
}

void deserialize(std::back_insert_iterator<std::string> value, StringConstIt& begin,
                 StringConstIt end) {
    if (!canDeserialize<std::size_t>(begin, end))
        throw std::runtime_error("Cannot correctly deserialize the message. Not enough bytes.");
    std::size_t size;
    deserialize(&size, begin, end);
    if (!canDeserialize(begin, end, size))
        throw std::runtime_error("Cannot correctly deserialize the message. Not enough bytes.");
    std::copy(begin, std::next(begin, size), value);
    std::advance(begin, size);
}

template <MsgType T>
std::unique_ptr<Message> deserialize(StringConstIt& begin, StringConstIt end) {
    return deserialize<T>(begin, end);
}

template <>
std::unique_ptr<Message> deserialize<TYPE_CONNECTION_MSG>(StringConstIt& begin,
                                                             StringConstIt end) {
    auto msg = std::make_unique<ConnectionMessage>();
    std::advance(begin, sizeof(MsgType));
    deserialize(&msg->connect, begin, end);
    return msg;
}

template <>
std::unique_ptr<Message> deserialize<TYPE_OPERATION_MSG>(StringConstIt& begin,
                                                           StringConstIt end) {
    auto msg = std::make_unique<OperationMessage>();
    std::advance(begin, sizeof(MsgType));
    deserialize(&msg->op_type, begin, end);
    deserialize(&msg->tuple_size, begin, end);
    return msg;
}

template <>
std::unique_ptr<Message> deserialize<TYPE_TUPLE_ELEM>(StringConstIt& begin,
                                                           StringConstIt end) {
    auto msg = std::make_unique<TupleElemMessage>();
    std::advance(begin, sizeof(MsgType));
    deserialize(&msg->elem_type, begin, end);
    int int_val;
    double float_val;
    std::string str_val;
    switch (msg->elem_type)
    {
        case ELEM_INT:
            deserialize(&int_val, begin, end);
            msg->elem = int_val;
            break;
        case ELEM_FLOAT:
            deserialize(&float_val, begin, end);
            msg->elem = float_val;
            break;
        case ELEM_STRING:
            deserialize(std::back_inserter(str_val), begin, end);
            msg->elem = str_val;
            break;
        default:
            throw std::runtime_error("Cannot deserialize. Unknown tuple elem type.");
    }
    return msg;
}

// find first message and move iterator to the beginning of it
MsgType findFirstMsgType(StringConstIt& begin, StringConstIt end) {
    auto NotFound = [begin, end]() { return (std::size_t)(end - begin); };
    std::map<MsgType, std::size_t> msg_positions = {{TYPE_CONNECTION_MSG, NotFound()},
                                                    {TYPE_OPERATION_MSG, NotFound()},
                                                    {TYPE_TUPLE_ELEM, NotFound()}
                                                    };
    for (auto& msg_pos : msg_positions) {  // for each type try to find it
        msg_pos.second = (std::size_t)(std::find(begin, end, (char)msg_pos.first) -
                                       begin);  // find - begin -> position
    }
    // find first message
    auto first_msg = std::min_element(msg_positions.begin(), msg_positions.end(),
                                      [NotFound](const auto& a, const auto& b) {
                                          return (a.second < b.second) && (a.second != NotFound());
                                      });
    if (first_msg == msg_positions.end() || first_msg->second == NotFound()) throw NoMessageFound();
    std::advance(begin,
                 first_msg->second);  // advance iterator to the beginning of the first message
    return first_msg->first;
}

std::unique_ptr<Message> deserialize(StringConstIt& begin, StringConstIt end) {
    auto msg_type = findFirstMsgType(begin, end);
    switch (msg_type) {
        case (TYPE_CONNECTION_MSG):
            return deserialize<TYPE_CONNECTION_MSG>(begin, end);
        case (TYPE_OPERATION_MSG):
            return deserialize<TYPE_OPERATION_MSG>(begin, end);
        case (TYPE_TUPLE_ELEM):
            return deserialize<TYPE_TUPLE_ELEM>(begin, end);
        default:
            throw std::runtime_error("Cannot deserialize. Unknown message type.");
    }
}
    
} // namespace Lindux
