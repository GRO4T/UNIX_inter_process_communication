echo "----------------TEST--------------------"
echo "Test client connect and disconnect"
INPUT_FILE=test/integration/client_connect_immediately_disconnect/input.txt
echo "Client commands:"
cat $INPUT_FILE && echo ""
bin/server > /dev/null 2>&1 &
sleep 1
bin/client test.log < $INPUT_FILE
killall bin/server
