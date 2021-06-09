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

void linda::bufferedReadFromPipe(MessageBuffer& msg_buffer, const int fifo_read) {
    auto bytes = readBytes(fifo_read);
    LOG_S(INFO) << fmt::format("Received {} bytes", bytes.length());
    msg_buffer.push(bytes);
}

void linda::sendTuple(OperationType op_type, const std::vector<TupleElem> tuple, const int fifo_write) {
    OperationMessage header(op_type, tuple.size());
    sendMessage(header, fifo_write);
    for (auto elem : tuple) {
        if (elem.index() == 0) {
            int val = std::get<int>(elem);
            TupleElemMessage msg(val);
            sendMessage(msg, fifo_write);
        }
    }
}
void linda::sendPattern(OperationType op_type, const std::vector<Pattern> pattern, const int fifo_write) {
    OperationMessage header(op_type, pattern.size());
    sendMessage(header, fifo_write);
    for (auto elem : pattern) {
        sendMessage(elem, fifo_write);
    }
}

std::unique_ptr<linda::Message> linda::readFromPipeUntilMessageFound(
    MessageBuffer& message_buffer, const int fifo_read) {
    std::optional<std::unique_ptr<Message>> msg_optional;
    while (true) {
        if ((msg_optional = fetchMessageFromBuffer(message_buffer)) &&
            msg_optional.has_value()) {
            return std::move(msg_optional.value());
        }
        bufferedReadFromPipe(message_buffer, fifo_read);
    }
}

std::vector<TupleElem> linda::receiveTuple(int tuple_length,
                                    MessageBuffer& message_buffer,
                                    const int fifo_read) {
    std::vector<TupleElem> tuple;
    int i = 0;
    while (i < tuple_length) {
        auto msg = readFromPipeUntilMessageFound(message_buffer, fifo_read);
        if (msg->GetType() != TYPE_TUPLE_ELEM)
            throw std::runtime_error("Expected tuple elem. Got something else\n");
        DLOG_S(INFO) << "Received tuple elem\n";
        auto elem_msg = static_cast<TupleElemMessage*>(msg.get());
        tuple.push_back(elem_msg->elem);
        i++;
    }
    return tuple;
}

std::vector<Pattern> linda::receivePattern(int tuple_length,
                                    MessageBuffer& message_buffer,
                                    const int fifo_read) {
    std::vector<Pattern> pattern_tuple;
    int i = 0;
    while (i < tuple_length){
        auto msg = readFromPipeUntilMessageFound(message_buffer, fifo_read);
        if (msg->GetType() != TYPE_TUPLE_PATTERN_ELEM)
            throw std::runtime_error("Expected tuple pattern elem. Got something else\n");
        DLOG_S(INFO) << "Received tuple pattern elem\n";
        auto pattern_msg = static_cast<Pattern*>(msg.get());
        pattern_tuple.push_back(*pattern_msg);
        i++;
    }
    return pattern_tuple;
}
