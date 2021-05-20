#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <variant>

namespace Lindux
{

// TODO move it somewhere else
using TupleElem = std::variant<int, double, std::string>;

using MsgType = uint8_t;
using ElemType = uint8_t;
using OperationType = uint8_t;

const MsgType TYPE_CONNECTION_MSG = 0b10000000;
const MsgType TYPE_OPERATION_HEADER = 0b01000000;
const MsgType TYPE_TUPLE_ELEM = 0b00100000;

const OperationType OP_LINDA_READ = 0b10000000;
const OperationType OP_LINDA_INPUT = 0b01000000;
const OperationType OP_LINDA_WRITE = 0b00100000;
const OperationType OP_RETURN_RESULT = 0b00010000;

const ElemType ELEM_INT = 0b10000000;
const ElemType ELEM_FLOAT = 0b01000000;
const ElemType ELEM_STRING = 0b00100000;

struct Message {
    virtual MsgType GetType() = 0;
};

struct ConnectionMessage : public Message {
    ConnectionMessage() {}
    ConnectionMessage(bool connect) : connect(connect) {}

    MsgType GetType() { return TYPE_CONNECTION_MSG; }

    bool connect;
};

struct OperationMessage : public Message {
    OperationMessage() {}
    OperationMessage(OperationType opType, uint32_t tupleSize)
        : op_type(opType), tuple_size(tupleSize) {}

    MsgType GetType() { return TYPE_OPERATION_HEADER; }

    OperationType op_type; 
    uint32_t tuple_size;
};

struct TupleElemMessage : public Message {
    TupleElemMessage() {}
    TupleElemMessage(ElemType elemType, const TupleElem& elem)
        : elem_type(elemType), elem(elem) {}

    MsgType GetType() { return TYPE_TUPLE_ELEM; }

    ElemType elem_type; 
    TupleElem elem;
};
    
} // namespace Lindux


#endif