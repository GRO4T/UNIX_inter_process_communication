#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace linda {
 uint32_t openFIFO(const char* path, int mode);
 void makeFIFO(const char* path, mode_t mode);
 void closeFIFO(int fileDescriptor);
}  // namespace linda
