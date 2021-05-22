#include "linda_functions/linda_functions.hpp"

int main(){
    linda::LindaFifoPaths paths = linda::lindaConnect();
    linda::linda_work(paths);
    return 0;
}