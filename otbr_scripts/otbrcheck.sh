echo "Check if serial device exists"
ls /dev/ttyACM0 
echo "----"

echo sudo ot-ctl state
sudo ot-ctl state 
echo "----"

echo otbr-agent state
sudo service otbr-agent status 
echo "----"

echo otbr-web state
sudo service otbr-web status
echo "----"
