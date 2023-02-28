# OpenThread Network Project

## Description of OpenThread 
Openthread is an open-source implementation of Thread, a network  layout to connect many devices which comes with many features, as it offers security, 6Lowpan and uses IEEE 802.15. 4 radio technology.
A border router is a device that is both connected to the Thread mesh Network and the external network, acting as a bridge between them.

## Steps for building/deploying Openthread Network
### Deploying Border Router
1. Go to https://openthread.io/guides/border-router/build (follow the Build and Configuration guide to build openthread on RPI).
2. Go to https://github.com/openthread/ot-nrf528xx and flash nordic dongle with ot-rcp.zip. 
3. Put nordic dongle into the RPI usb port - needed for thread network.
### Deploying Simple-End Device:
1. Go to https://github.com/openthread/ot-nrf528xx and flash another nordic dongle with ot-cli-ftd.zip.
### Initiating Thread Network
1. Go to https://openthread.io/guides/border-router/docker/test-connectivity and follow steps and you should have a working thread network.
#### Tips
1. To view logs from the otbr services:

```journalctl -u otbr-agent.service ```

```journalctl -u otbr-web.service```

2. **Important**: Links to otbr-scripts https://github.com/umgop/OpenThreadAutoJoin/tree/master/otbr_scripts:

On the RPI, writing scripts to check that openthread is running properly is important. When otbr web runs simuntaneosly with the other otbr components, the otbr-agent doesn't start properly. To solve this, I created a otbr-start.sh script that stores information to the out.text file about if the nordic dongle(connected to RPI) is present, if otbr-agent is running and if the border router is part of a network. The check.py script checks the out.text.file to see the output and then starts the otbr-web if the output matches.This ensures that openthread is launched properly and in th correct order.

3. To check if simple-end devices are connected, go to topology in localhost tab and see if there is a device connected to the border router.

4. If the screen command isn’t working try reflashing.



**Important**: Use `ls /dev/tty*` to check if your nordic dongle is connected to your laptop

### Additions to Openthread Network
### Auto-Joining Routers
**Code for all functionalities**: https://github.com/umgop/OpenThreadAutoJoin/blob/master/openthread/examples/apps/cli/main.c

1. Code for ot-nrf528xx repo’s main.c file to give the nordic dongle (router) auto-joining network functionalities (now you shouldn’t need to use screen and input the data of the network yourself). https://github.com/umgop/OpenThreadAutoJoin/blob/a17dc4af187ca7129486ae3cb21ee84f11371cf2/openthread/examples/apps/cli/main.c#L155 
 	
	a. When defining the setNetworkConfiguration function, only define the network key - nothing else - to join as router.
	
	b. To force your devices to join as a router - also use the oTBecomeRouter function (make sure to use after thread is up).
2. Linking buttons/leds to nordic dongles: follow full guide  https://openthread.io/codelabs/openthread-apis#0. 
3. Coap Server: This will just be a place to maybe display data that your routers/sensors gather.
https://aniotodyssey.com/2021/06/12/creating-a-simple-coap-server-with-python.html  - set up Coap Server on RPI
4. Add coap-sending command to ot-nrf528xx repo’s main.c file, to give nordic dongle coap sending functionality.  https://github.com/umgop/OpenThreadAutoJoin/blob/a17dc4af187ca7129486ae3cb21ee84f11371cf2/openthread/examples/apps/cli/main.c#L234




## FAQ:
 
1. *What is the openthread infrastructure in the lab?*\
	The openthread infrastructure for the lab consists of a few sensors (permamotes), nrf8540 dongles(serving as routers) and a Raspberry Pi with another nrf8540 dongle (serving as the NCP). 
 
 
2. *How do I connect a device to the network?*\
	To add devices to network you have to flash code depending on the function you want to perform (Go to https://github.com/openthread/ot-nrf528xx for base code).
 
	
3. *How do I debug a problem if the network isn’t working?*\
	In the scenario the thread network doesn’t work:
Try reconnecting the nrf dongle and restarting the RPI.\
Try restarting openthread on the RPI by running,  `sudo ot-ctl restart otbr-agent` or`sudo ot-ctl restart otbr-web`.
 
 
4. *How can I get my data out of the network?*\
Coap Server: The nordic dongles after recieving information can send a coap message to the RPI coap server on the data recieved. The data of the network will be displayed on the Desktop Monitor connected to the RPI which is running a coap server.
 
 
5. *How can I talk back to my devices?*\
	To find and send data to devices in the network you can use terminal commands on the RPI such as “router table” to get all the information on the routers in the network(rloc, mac addr etc.) or “child table” to get info on the child devices in the network.
 
