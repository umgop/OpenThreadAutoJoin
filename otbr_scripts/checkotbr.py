# Write your code here :-)
import os
import time

counter = 0

def checkfunction():
    with open('/home/umgop/out.txt') as f:
        lines = f.readlines()

    if '/dev/ttyACM0' in lines[0]:
        state = 1
    else:
        state = 0
        print("Cannot find tty")

    if 'child' in lines[1]:
            state = 1
    elif 'router' in lines[1]:
        state = 1
    elif 'leader' in lines[1]:
        state = 1
    else:
        state = 0
        print(lines[1])

    if  'Active: active (running)' in lines[5]:
        state = 1
    else:
        state = 0
        print(lines[5])

    return state

while checkfunction() == 0 and counter < 20:
    time.sleep(3)
    counter = counter +1



if checkfunction() == 1:
    print("Sucessfull")
    os.system('sudo systemctl restart otbr-web')




