echo "----------------TEST--------------------"
echo "Test read after output for single client"
echo "Server should return (1, 2) tuple"
INPUT_FILE=test/integration/read_after_write/input.txt
echo "Client commands:"
cat $INPUT_FILE && echo ""
rm /dev/shm/sem.bus_mutex && rm /tmp/linda_bus_write && rm /tmp/linda_bus_read
bin/server > /dev/null 2>&1 &
sleep 1
bin/client test.log < $INPUT_FILE
killall bin/server
