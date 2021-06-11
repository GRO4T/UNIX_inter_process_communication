INPUT_FILE=test/integration/wait_with_read/input2.txt
sleep 1
echo "Client B starting job"
bin/client test.log < $INPUT_FILE
echo "Client B ending job"
killall bin/server

