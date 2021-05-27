#include "linda_common.hpp"

#include "common.hpp"

uint32_t linda::openFIFO(const std::string& path, int mode) {
    int fd = open(path.c_str(), mode);
    if (fd < 0)
        LOG_S(ERROR) << "Cannot open FIFO\n";
    return fd;
}

void linda::makeFIFO(const std::string& path, mode_t mode) {
    int result = mkfifo(path.c_str(), mode);
    if (result < 0)
        LOG_S(ERROR) << "Error while creating FIFO\n";
}

void linda::closeFIFO(int fileDescriptor) { close(fileDescriptor); }


void linda::sendBytes(const std::string& msg, const int fifo_fd) {
    int ret = write(fifo_fd, msg.c_str(), msg.length());
    if (ret < 0)
        throw "Error: Could not sendBytes through FIFO.";
}

std::string linda::readBytes(const int fifo_fd) {
    char buf[BUFFER_SIZE];
    int bytes_recvd = read(fifo_fd, buf, BUFFER_SIZE);
    if (bytes_recvd < 0) {
        LOG_S(ERROR) << "Error while reading fifo paths\n";
    }
    return {buf, (ulong)bytes_recvd};
}