echo "----------------TEST--------------------"
echo "Test read after output for single client"
echo "Server should return (1, 2) tuple"
rm /dev/shm/sem.bus_mutex && rm /tmp/linda_bus_write && rm /tmp/linda_bus_read
bin/server > /dev/null 2>&1 &
sleep 1
bin/client test.log < test/integration/read_after_write/input.txt