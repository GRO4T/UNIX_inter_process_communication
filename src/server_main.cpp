#include <iostream>

#include "linda_server/linda_server.hpp"

int main() {
    std::cout << "Hello LINDUX" << std::endl;
    linda::LindaServer serv;
    serv.mainLoop();
    return 0;
}