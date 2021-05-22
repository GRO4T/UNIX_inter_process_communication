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
inline uint32_t openFIFO(const char* path, int mode) {
    int fd = open(path, mode);
    if (fd < 0) perror("Cannot open FIFO");
    return fd;
}

inline void makeFIFO(const char* path, mode_t mode) {
    int result = mkfifo(path, mode);
    if (result < 0) perror("Error while creating FIFO");
}

inline void closeFIFO(int fileDescriptor) { close(fileDescriptor); }
}  // namespace linda
