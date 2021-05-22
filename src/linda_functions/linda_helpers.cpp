#include "linda_functions/linda_helpers.hpp"

uint32_t linda::openFIFO(const char* path, int mode) {
    int fd = open(path, mode);
    if (fd < 0) perror("Cannot open FIFO");
    return fd;
}

void linda::makeFIFO(const char* path, mode_t mode) {
    int result = mkfifo(path, mode);
    if (result < 0) perror("Error while creating FIFO");
}

void linda::closeFIFO(int fileDescriptor) { close(fileDescriptor); }