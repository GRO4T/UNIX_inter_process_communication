echo "----------------TEST--------------------"
echo "Test read after output for single client"
echo "Server should return (1, 2) tuple"
INPUT_FILE=test/integration/read_after_write/input.txt
echo "Client commands:"
cat $INPUT_FILE && echo ""
bin/server > /dev/null 2>&1 &
sleep 1
bin/client test.log < $INPUT_FILE
killall bin/server
