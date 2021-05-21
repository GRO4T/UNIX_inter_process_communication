#pragma once
#include <cstdint>
namespace linda {

struct __attribute__((__packed__)) LindaMessage {
    uint8_t control_mask;
    int tuple_integer;
    double tuple_float;
    char tuple_string[100];
};

struct LindaFifoPaths {
    char read_path[255];
    char write_path[255];
};

namespace commands {
constexpr uint8_t connect = 0b10000000;
constexpr uint8_t send_tuple = 0b01000000;
constexpr uint8_t linda_read = 0b00100000;
constexpr uint8_t linda_input = 0b00010000;
constexpr uint8_t linda_output = 0b00001000;
constexpr uint8_t type_int = 0b00000100;
constexpr uint8_t type_float = 0b00000010;
constexpr uint8_t type_string = 0b00000001;
}  // namespace commands

namespace consts {
constexpr char linda_bus_write[] = "/tmp/linda_bus_write";
constexpr char linda_bus_read[] = "/tmp/linda_bus_read";
constexpr char bus_mutex[] = "/bus_mutex";
constexpr uint max_path = 255;
constexpr int mutex_value = 1;
constexpr int perms = 0666;
constexpr int message_size = sizeof(linda::LindaMessage);
}  // namespace consts

}  // namespace linda