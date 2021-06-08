#include "linda_common.hpp"

#include "common.hpp"
#include "deserializer.hpp"

using namespace linda;

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

std::string linda::readBytes(const int fifo_fd) {
    char buf[PIPE_READ_SIZE];
    int bytes_recvd = read(fifo_fd, buf, PIPE_READ_SIZE);
    if (bytes_recvd < 0) {
        LOG_S(ERROR) << "Error while reading fifo paths\n";
    }
    return {buf, (ulong)bytes_recvd};
}

std::optional<std::unique_ptr<Message>> linda::fetchMessageFromBuffer(
    linda::MessageBuffer& msg_buffer) {
    // try to obtain message bytes from buffer
    auto message_optional = msg_buffer.popMessage();
    if (!message_optional.has_value()) return std::nullopt;
    auto bytes = message_optional.value();
    auto c_it = bytes.cbegin();
    try {
        auto recv_msg = deserialize(c_it, bytes.cend());
        return std::optional<std::unique_ptr<linda::Message>>(std::move(recv_msg));
    }
    catch (std::exception& e) {
        LOG_S(ERROR) << fmt::format("Error deserializing message: {}", e.what());
        return std::nullopt;
    }
}

void linda::bufferedReadFromPipe(MessageBuffer& msg_buffer, const int fifo_fd) {
    LOG_S(INFO) << "Received bytes...\n";
    auto bytes = readBytes(fifo_fd);
    msg_buffer.push(bytes);
}

