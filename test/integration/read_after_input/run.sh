echo "----------------TEST--------------------"
echo "Test read after input for single client"
echo "Input should return (1, 2). Then read should hang."
INPUT_FILE=test/integration/read_after_input/input.txt
echo "Client commands:"
cat $INPUT_FILE && echo ""
rm /dev/shm/sem.bus_mutex && rm /tmp/linda_bus_write && rm /tmp/linda_bus_read
bin/server > /dev/null 2>&1 &
sleep 1
bin/client test.log < test/integration/read_after_input/input.txt &
sleep 2 && echo "TEST: terminating hanging client" && killall bin/client
killall bin/server