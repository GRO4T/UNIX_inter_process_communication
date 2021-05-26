#include "common.hpp"
#include "linda_functions.hpp"

int main(int argc, char ** argv){
    linda::loggerInit(argc, argv);
    linda::LindaFifoPaths paths = linda::lindaConnect();
    return 0;
}