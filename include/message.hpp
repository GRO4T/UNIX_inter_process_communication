#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <variant>

namespace linda {

// TODO move it somewhere else
using TupleElem = std::variant<int, double, std::string>;

using MsgType = uint8_t;
using ElemType = uint8_t;
using OperationType = uint8_t;

const MsgType TYPE_CONNECTION_MSG = 0b10000000;
const MsgType TYPE_OPERATION_MSG = 0b01000000;
const MsgType TYPE_TUPLE_ELEM = 0b00100000;
const MsgType TYPE_SERVER_CONN_RESPONSE = 0b00010000;

const OperationType OP_LINDA_READ = 0b10000000;
const OperationType OP_LINDA_INPUT = 0b01000000;
const OperationType OP_LINDA_WRITE = 0b00100000;
const OperationType OP_RETURN_RESULT = 0b00010000;

// they correspond to indices in TupleElem variant
const ElemType ELEM_INT = 0;
const ElemType ELEM_FLOAT = 1;
const ElemType ELEM_STRING = 2;

struct Message {
    virtual MsgType GetType() = 0;
};

struct ConnectionMessage : public Message {
    ConnectionMessage() {}
    ConnectionMessage(bool connect) : connect(connect) {}

    MsgType GetType() { return TYPE_CONNECTION_MSG; }

    bool connect;
};

struct ServerConnectionResponse : public Message {
    ServerConnectionResponse() {}
    ServerConnectionResponse(bool connected, const std::string& fifo_write,
                             const std::string& fifo_read)
        : connected(connected), fifo_write(fifo_write), fifo_read(fifo_read) {}

    MsgType GetType() { return TYPE_SERVER_CONN_RESPONSE; }
    bool connected;
    std::string fifo_write;
    std::string fifo_read;
};

struct OperationMessage : public Message {
    OperationMessage() {}
    OperationMessage(OperationType opType, uint32_t tupleSize)
        : op_type(opType), tuple_size(tupleSize) {}

    MsgType GetType() { return TYPE_OPERATION_MSG; }

    OperationType op_type;
    uint32_t tuple_size;
};

struct TupleElemMessage : public Message {
    TupleElemMessage() {}
    TupleElemMessage(int val) : elem_type(ELEM_INT), elem(val) {}
    TupleElemMessage(double val) : elem_type(ELEM_FLOAT), elem(val) {}
    TupleElemMessage(std::string val) : elem_type(ELEM_STRING), elem(val) {}

    MsgType GetType() { return TYPE_TUPLE_ELEM; }

    ElemType elem_type;
    TupleElem elem;
};

}  // namespace linda

#endif