#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "linda_spec.hpp"
linda::LindaFifoPaths lindaConnect() {
    sem_t* busSem = sem_open(linda::consts::bus_mutex, 0);

    if (busSem == nullptr) {
        perror("Error while opening semaphore");
    }
    sem_wait(busSem);

    int fifoRead = open(linda::consts::linda_bus_read, O_RDWR);
    if (fifoRead < 0) perror("Error while opening FIFO");
    int fifoWrite = open(linda::consts::linda_bus_write, O_RDWR);
    if (fifoRead < 0) perror("Error while opening FIFO");

    linda::LindaFifoPaths paths;
    memset(&paths, 0, sizeof(paths));

    linda::LindaMessage msg{};
    msg.control_mask = linda::commands::connect;
    write(fifoWrite, &msg, linda::consts::message_size);

    int result = read(fifoRead, &paths, sizeof(linda::LindaFifoPaths));
    if (result < 0 || result != sizeof(linda::LindaFifoPaths)) {
        std::cout << "Już za chwilę" << std::endl;
        std::cout << result << std::endl;
        perror("Error while reading fifo paths");
    }

    std::cout << "READ: " << paths.read_path << std::endl;
    std::cout << "WRITE: " << paths.write_path << std::endl;

    close(fifoRead);
    close(fifoWrite);
    sem_post(busSem);
    sem_close(busSem);
    return paths;
}
