echo "----------------TEST--------------------"
echo "Test client connect and disconnect"
rm /dev/shm/sem.bus_mutex && rm /tmp/linda_bus_write && rm /tmp/linda_bus_read
bin/server > /dev/null 2>&1 &
sleep 1
bin/client test.log < test/integration/client_connect_immediately_disconnect/input.txt
killall bin/server
