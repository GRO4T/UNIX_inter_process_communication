#include "client.hpp"
#include "common.hpp"

int main(int argc, char ** argv){
    try {
        linda::loggerInit(argc, argv);
        linda::Client client;
        client.connect();
    }
    catch (std::exception& e) {
        LOG_S(ERROR) << e.what() << std::endl;
    }
    return 0;
}