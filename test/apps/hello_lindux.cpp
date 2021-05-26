#include <iostream>
#include "dummy.hpp"
#include "common.hpp"
#include "message.hpp"

#include "serializer.hpp"
#include "deserializer.hpp"

using namespace linda;

int main(int argc, char ** argv) {
    loggerInit(argc, argv); 
    LOG_S(INFO) << fmt::format("Hello I'm a formatted text {}", 1234);
    ServerConnectionResponse msg(true, "fifo_write", "fifo_read");
    auto bytes = serialize(msg);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    return 0;
}