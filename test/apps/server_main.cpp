#include <iostream>

#include "common.hpp"
#include "server.hpp"

int main(int argc, char ** argv) {
    linda::loggerInit(argc, argv);
    std::cout << "Hello LINDUX" << std::endl;
    linda::LindaServer serv;
    serv.mainLoop();
    return 0;
}