#include "client.hpp"
#include "common.hpp"


int main(int argc, char ** argv){
    std::string logfile = "client.log";
    if (argc > 1) {
        logfile = argv[1];
        std::cout << logfile;
    }

    try {
        linda::loggerInit(argc, argv, logfile);
        linda::Client client;
        client.connect();
    }
    catch (std::exception& e) {
        LOG_S(ERROR) << e.what() << std::endl;
    }
    return 0;
}