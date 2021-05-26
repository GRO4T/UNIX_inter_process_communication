#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#define BUFFER_SIZE 512

namespace linda {
 uint32_t openFIFO(const std::string& path, int mode);
 void makeFIFO(const std::string& path, mode_t mode);
 void closeFIFO(int fileDescriptor);


void sendBytes(const std::string& msg, const int fifo_fd);
std::string readBytes(const int fifo_fd);
}  // namespace linda
