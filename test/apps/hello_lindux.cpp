#include <iostream>
#include "dummy.hpp"
#include "common.hpp"

using namespace Lindux;

int main(int argc, char ** argv) {
    loggerInit(argc, argv); 
    LOG_S(INFO) << fmt::format("Hello I'm a formatted text {}", 1234);
    Dummy dummy; 
    dummy.hello();
    return 0;
}