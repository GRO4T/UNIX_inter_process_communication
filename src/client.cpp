#include "client.hpp"

#include "common.hpp"
#include "deserializer.hpp"
#include "linda_common.hpp"
#include "tuple.hpp"
#include "parser.hpp"

using namespace linda;

void Client::interact(){
    std::optional<std::unique_ptr<Message>> msg_optional;
    std::unique_ptr<Message> recv_msg;
    std::cout << "> ";
    while (true) {
        std::string user_command;
        if (!std::getline(std::cin, user_command)) continue;
        if (user_command == "exit") break;
        else if (user_command == "help") {
            std::cout << "possible commands:\ninput\noutput\nread\nexit\n";
            continue;
        }
        try {
            auto linda_command = parse(user_command);
            handleCommand(linda_command);
            auto recv_msg = readFromPipeUntilMessageFound(message_buffer, fifo_read);
            if(recv_msg->GetType() == linda::TYPE_OPERATION_MSG){
                auto server_response = static_cast<OperationMessage*>(recv_msg.get());
                if (server_response->op_type == OP_RETURN_RESULT) {
                    if (linda_command.first == OP_LINDA_WRITE && server_response->tuple_size == 0) {
                        LOG_S(INFO) << "Write successful";
                        std::cout << "Write successful\n";
                    }
                    else {
                        auto tuple = receiveTuple(server_response->tuple_size, message_buffer, fifo_read);
                        DLOG_S(INFO) << fmt::format("Received tuple {}", show(tuple));
                        std::cout << show(tuple) << std::endl;
                    }
                }
                else {
                    LOG_S(INFO) << "Received response from server, but not of expected type";
                    std::cout << "Server error\n";
                }
            }
        }
        catch (std::exception& e) {
            LOG_S(ERROR) << fmt::format("Error parsing user input: {}", e.what());
            std::cout << e.what() << std::endl;
        }
        std::cout << "> ";
    }

    disconnect();
}

void Client::handleCommand(Command cmd) {
    auto op = cmd.first;
    if (op == OP_LINDA_WRITE) {
        std::vector<TupleElem> tuple;
        for (auto elem : cmd.second) {
            tuple.push_back(std::get<TupleElem>(elem));
        }
        sendTuple(op, tuple, fifo_write);
    }
    else {
        std::vector<Pattern> pattern;
        for (auto elem : cmd.second) {
            pattern.push_back(std::get<Pattern>(elem));
        }
        sendPattern(op, pattern, fifo_write);
    }
}

void linda::Client::disconnect() {
    ConnectionMessage msg(false);
    sendMessage(msg, fifo_write);
    LOG_S(INFO) << "Disconnected";
}

void linda::Client::connect() {
    // connect to linda bus
    sem_t* bus_sem = sem_open(consts::bus_mutex, 0);
    if (bus_sem == nullptr)
        LOG_S(ERROR) << "Error while opening semaphore\n";
    sem_wait(bus_sem);
    int bus_read = openFIFO(consts::linda_bus_read, O_RDONLY);
    int bus_write = openFIFO(consts::linda_bus_write, O_WRONLY);
    // send connection request
    LOG_S(INFO) << "Client trying to connect...\n";
    ConnectionMessage msg(true);
    sendMessage(msg, bus_write);
    // Receive connection response with FIFO names
    auto bytes = readBytes(bus_read);
    auto c_it = bytes.cbegin();
    auto recv_msg = deserialize(c_it, bytes.cend());
    if (recv_msg->GetType() == TYPE_SERVER_CONN_RESPONSE) {
        auto recv_conn_resp_msg = static_cast<ServerConnectionResponse*>(recv_msg.get());
        if (!recv_conn_resp_msg->connected)
            throw std::runtime_error("Client could not connect!");
        LOG_S(INFO) << "Client connected...\n";
        read_path = recv_conn_resp_msg->fifo_read;
        write_path = recv_conn_resp_msg->fifo_write;
        DLOG_S(INFO) << "FIFO READ: " << read_path << std::endl;
        DLOG_S(INFO) << "FIFO WRITE: " << write_path << std::endl;
        sem_post(bus_sem);
        sem_close(bus_sem);
        fifo_read = linda::openFIFO(read_path, O_RDWR);
        fifo_write = linda::openFIFO(write_path, O_RDWR);
        interact();
    }
    else {
        throw std::runtime_error("Bad message type in this context. Expected ServerConnectionResponse!");
    }
    // tidy up
    closeFIFO(bus_read);
    closeFIFO(bus_write);
}
