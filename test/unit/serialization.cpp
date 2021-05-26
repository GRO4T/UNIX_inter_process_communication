#include <gtest/gtest.h>

#include <cstring>
#include <type_traits>

#include "deserializer.hpp"
#include "message.hpp"
#include "serializer.hpp"

using namespace linda;

TEST(Serialization, OneConnectionMessage) {
    ConnectionMessage msg(true);
    auto bytes = serialize(msg);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    EXPECT_EQ(recv_msg->GetType(), msg.GetType());
    auto recv_conn_msg = static_cast<ConnectionMessage*>(recv_msg.get());
    EXPECT_EQ(recv_conn_msg->connect, msg.connect);
}

TEST(Serialization, OneServerConnectionResponse) {
    ServerConnectionResponse msg(true, "fifo_write", "fifo_read");
    auto bytes = serialize(msg);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    EXPECT_EQ(recv_msg->GetType(), msg.GetType());
    auto recv_conn_msg = static_cast<ServerConnectionResponse*>(recv_msg.get());
    EXPECT_EQ(recv_conn_msg->connected, msg.connected);
    EXPECT_EQ(recv_conn_msg->fifo_write, msg.fifo_write);
    EXPECT_EQ(recv_conn_msg->fifo_read, msg.fifo_read);
}

TEST(Serialization, OneOperationMessage) {
    OperationMessage msg(OP_LINDA_READ, 123);
    auto bytes = serialize(msg);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    EXPECT_EQ(recv_msg->GetType(), msg.GetType());
    auto recv_op_msg = static_cast<OperationMessage*>(recv_msg.get());
    EXPECT_EQ(recv_op_msg->op_type, msg.op_type);
    EXPECT_EQ(recv_op_msg->tuple_size, msg.tuple_size);
}

TEST(Serialization, OneTupleElemInt) {
    TupleElemMessage msg(123);
    auto bytes = serialize(msg);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    EXPECT_EQ(recv_msg->GetType(), msg.GetType());
    auto recv_tuple = static_cast<TupleElemMessage*>(recv_msg.get());
    EXPECT_EQ(recv_tuple->elem_type, ELEM_INT);
    EXPECT_EQ(std::get<int>(recv_tuple->elem), 123);
}

TEST(Serialization, OneTupleElemFloat) {
    TupleElemMessage msg(123.5);
    auto bytes = serialize(msg);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    EXPECT_EQ(recv_msg->GetType(), msg.GetType());
    auto recv_tuple = static_cast<TupleElemMessage*>(recv_msg.get());
    EXPECT_EQ(recv_tuple->elem_type, ELEM_FLOAT);
    EXPECT_EQ(std::get<double>(recv_tuple->elem), 123.5);
}

TEST(Serialization, OneTupleElemString) {
    TupleElemMessage msg("test");
    auto bytes = serialize(msg);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    EXPECT_EQ(recv_msg->GetType(), msg.GetType());
    auto recv_tuple = static_cast<TupleElemMessage*>(recv_msg.get());
    EXPECT_EQ(recv_tuple->elem_type, ELEM_STRING);
    EXPECT_EQ(std::get<std::string>(recv_tuple->elem), "test");
}