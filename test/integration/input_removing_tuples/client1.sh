echo "-----------------TEST-------------------"
echo "Checking if command Input removes tuples"
echo "Client A writes one tuple, then uses Input twice"
echo "Expected outcome: Client A should be stuck in waiting queue after second input"
echo 
INPUT_FILE=test/integration/input_removing_tuples/input.txt
bin/server > /dev/null 2>&1 &
sleep 1
echo "Client A starting job"
bin/client test.log < $INPUT_FILE
echo "Client A ended job"
killall bin/server
