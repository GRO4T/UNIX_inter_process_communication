INPUT_FILE=test/integration/input_removing_tuples/input2.txt
sleep 3
echo "Client B writing something so Client A could wake up..."
bin/client test.log < $INPUT_FILE
echo "Client B ending job"
