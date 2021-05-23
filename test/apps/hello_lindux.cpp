#include <iostream>
#include "common.hpp"
#include "message.hpp"

using namespace Lindux;

int main(int argc, char ** argv) {
    loggerInit(argc, argv); 
    LOG_S(INFO) << fmt::format("Hello I'm a formatted text {}", 1234);
    return 0;
}