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
    std::cout << "WITAM WSZYSTKICH TO KLIENT" << std::endl;
    sem_t* busSem = sem_open(linda::consts::bus_mutex, 0);
    // sem_t* serverSem = sem_open(linda::consts::inter_server_mutex, 0);

    if (busSem == nullptr) {
        perror("Error while opening semaphore");
    }
    std::cout<<"PRZED READEM"<<std::endl;
    sem_wait(busSem);

    int fifoFd = open(linda::consts::linda_bus, O_RDWR);
    if (fifoFd < 0) perror("Error while opening FIFO");

    struct pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = fifoFd;
    pfd.events = POLLIN | POLLOUT;

    int ret = 0;

    while (ret <= 0 && !(pfd.revents & POLLOUT)) {
        ret = poll(&pfd, 1, -1);
    }

    linda::LindaFifoPaths paths;
    ret = 0;
    memset(&paths, 0, sizeof(paths));

    linda::LindaMessage msg{};
    msg.control_mask = linda::commands::connect;
    std::cout<<"PRZED WRITEM"<<std::endl;
    write(fifoFd, &msg, linda::consts::message_size);
    
    // sem_post(serverSem);
    std::cout<<"PRZED READEM"<<std::endl;
    int result = read(fifoFd, &paths, sizeof(linda::LindaFifoPaths));
    if (result < 0 || result != sizeof(linda::LindaFifoPaths)) {
        std::cout<<"Już za chwilę"<<std::endl;
        std::cout<<result<<std::endl;
        perror("Error while reading fifo paths");
    }

    std::cout << "READ: " << paths.read_path << std::endl;
    std::cout << "WRITE: " << paths.write_path << std::endl;

    close(fifoFd);
    sem_post(busSem);
    sem_close(busSem);
    // sem_close(serverSem);
    return paths;
}
