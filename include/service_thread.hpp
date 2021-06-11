#pragma once
#include <memory>
#include <vector>
#include "linda_common.hpp"
#include "message.hpp"

namespace linda{
class ServerDB;

struct AwaitingParams{
    AwaitingParams() {}
    AwaitingParams(std::vector<Pattern> _tuple_pattern, bool _isInput) 
    : tuple_pattern(_tuple_pattern), isInput(_isInput) {} 

    std::vector<Pattern> tuple_pattern;
    std::mutex mutex;
    bool isInput;
    std::vector<TupleElem> passed_tuple;
};

struct ServiceThreadParameters{
    ServiceThreadParameters(FifoPaths _paths, ServerDB* _datebase) : 
        paths(_paths), databasePtr(_datebase) {}

    FifoPaths paths;
    ServerDB* databasePtr;
};

class ServiceThread{
public:
    ServiceThread(ServiceThreadParameters params);
    static void* mainLoop(void* arg);

private:
    bool handleConnectionMessage(Message* msg);
    void handleOperationMessage(OperationMessage* msg);
    void handleRead(int tuple_length);
    void handleInput(int tuple_length);
    void handleWrite(int tuple_length);

    ServerDB* database;
    int32_t fifo_read;
    int32_t fifo_write;

    AwaitingParams awaiting_parameters;
    MessageBuffer message_buffer;
};

} //namespace linda