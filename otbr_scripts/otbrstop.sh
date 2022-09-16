echo "Stop otbr"
sudo service otbr-agent stop
sudo service otbr-web stop
sudo ot-ctl reset
sudo ot-ctl state
echo -----

