# Lindux
## Set up
```
git submodule update --init
mkdir build
cd build
cmake ..
make
```

## Semaphores
```
rm /dev/shm/sem.bus_mutex && rm /tmp/linda_bus_write && rm /tmp/linda_bus_read
```