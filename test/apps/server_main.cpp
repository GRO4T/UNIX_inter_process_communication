#include <iostream>

#include "common.hpp"
#include "server.hpp"

int main(int argc, char ** argv) {
    try {
        linda::loggerInit(argc, argv);
        linda::Server serv;
        serv.mainLoop();
    }
    catch (std::exception& e) {
        LOG_S(ERROR) << e.what() << std::endl;
    }
    return 0;
}