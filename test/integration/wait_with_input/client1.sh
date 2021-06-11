echo "-----------------TEST-------------------"
echo "Waiting for tuple test - command INPUT"
echo "Client B starts first but he has to wait for tuple"
echo "Expected outcome: Client A should end his work first, then Client B"
echo
INPUT_FILE=test/integration/wait_with_input/input.txt
rm /dev/shm/sem.bus_mutex && rm /tmp/linda_bus_write && rm /tmp/linda_bus_read
bin/server > /dev/null 2>&1 &
sleep 3
echo "Client A starting job"
bin/client test.log < $INPUT_FILE
echo "Client A ended job"
