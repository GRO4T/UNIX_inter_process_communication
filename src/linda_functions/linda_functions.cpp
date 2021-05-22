#include "linda_functions/linda_functions.hpp"

linda::LindaFifoPaths linda::lindaConnect() {
    sem_t* busSem = sem_open(consts::bus_mutex, 0);

    if (busSem == nullptr) {
        perror("Error while opening semaphore");
    }
    sem_wait(busSem);

    int fifoRead = openFIFO(consts::linda_bus_read, O_RDONLY);
    int fifoWrite = openFIFO(consts::linda_bus_write, O_WRONLY);

    LindaFifoPaths paths;
    memset(&paths, 0, sizeof(paths));

    LindaMessage msg{};
    msg.control_mask = commands::connect;
    write(fifoWrite, &msg, consts::message_size);

    int result = read(fifoRead, &paths, sizeof(LindaFifoPaths));
    if (result < 0 || result != sizeof(LindaFifoPaths)) {
        std::cout << result << std::endl;
        perror("Error while reading fifo paths");
    }

    std::cout << "READ: " << paths.read_path << std::endl;
    std::cout << "WRITE: " << paths.write_path << std::endl;

    closeFIFO(fifoRead);
    closeFIFO(fifoWrite);
    sem_post(busSem);
    sem_close(busSem);
    return paths;
}
void linda::linda_work(LindaFifoPaths paths) {
    int fifoWrite = openFIFO(paths.write_path, O_WRONLY);
    int fifoRead = openFIFO(paths.read_path, O_RDONLY);

    char buffer[linda::consts::max_path] = "Hello from client!";
    write(fifoWrite, &buffer, sizeof(buffer));

    char msg[linda::consts::max_path];
    memset(&msg, 0, sizeof(msg));
    read(fifoRead, &msg, sizeof(msg));
    std::cout << msg << std::endl;

    closeFIFO(fifoRead);
    closeFIFO(fifoWrite);
}