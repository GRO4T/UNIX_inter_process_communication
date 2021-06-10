echo "----------------TEST--------------------"
echo "Test client connect and disconnect"
INPUT_FILE=test/integration/client_connect_immediately_disconnect/input.txt
echo "Client commands:"
cat $INPUT_FILE && echo ""
rm /dev/shm/sem.bus_mutex && rm /tmp/linda_bus_write && rm /tmp/linda_bus_read
bin/server > /dev/null 2>&1 &
sleep 1
bin/client test.log < $INPUT_FILE
killall bin/server
