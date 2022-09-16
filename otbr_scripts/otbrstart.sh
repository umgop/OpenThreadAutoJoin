rm out.txt

echo "Check if serial device exists"
ls /dev/ttyACM0  > out.txt
echo "----"

echo sudo ot-ctl state
sudo ot-ctl state >> out.txt
echo "----"

echo otbr-agent state
sudo service otbr-agent status >> out.txt
echo "----"

python3 /home/umgop/mu_code/checkotbr.py
