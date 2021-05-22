#pragma once
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "linda_helpers.hpp"
#include "linda_spec.hpp"

namespace linda {
LindaFifoPaths lindaConnect();
void linda_work(LindaFifoPaths paths);
}  // namespace linda
