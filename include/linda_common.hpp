#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <string>
#include <stdexcept>

#define BUFFER_SIZE 512

namespace linda {

struct FifoPaths {
    FifoPaths(const std::string& read_path, const std::string& write_path) {
        if (read_path.length() >= 255 || write_path.length() >= 255)
            throw std::runtime_error("Path too long. Max length 255.");
        strcpy(this->write_path, write_path.c_str());
        strcpy(this->read_path, read_path.c_str());
    }

    char read_path[255];
    char write_path[255];
};

namespace consts {
constexpr char linda_bus_write[] = "/tmp/linda_bus_write";
constexpr char linda_bus_read[] = "/tmp/linda_bus_read";
constexpr char bus_mutex[] = "/bus_mutex";
constexpr uint max_path = 255;
constexpr int mutex_value = 1;
constexpr int perms = 0666;
}  // namespace consts

uint32_t openFIFO(const std::string& path, int mode);
void makeFIFO(const std::string& path, mode_t mode);
void closeFIFO(int fileDescriptor);

void sendBytes(const std::string& msg, const int fifo_fd);
std::string readBytes(const int fifo_fd);
}  // namespace linda
