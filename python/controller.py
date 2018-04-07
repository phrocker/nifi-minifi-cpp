#!/usr/bin/env python
import sys
import os.path

sys.path.append(os.path.join(os.path.dirname('/home/pi/deploy/python/'), '..'))
import RPi.GPIO as GPIO
import video_dir
import car_dir
import motor

from time import ctime          # Import necessary modules   

ctrl_cmd = ['forward', 'backward', 'left', 'right', 'stop', 'read cpu_temp', 'home', 'distance', 'x+', 'x-', 'y+', 'y-', 'xy_home']

busnum = 1          # Edit busnum to 0, if you uses Raspberry Pi 1 or 0

video_dir.setup(busnum=busnum)
car_dir.setup(busnum=busnum)
motor.setup(busnum=busnum)     # Initialize the Raspberry Pi GPIO connected to the DC motor. 
video_dir.home_x_y()
car_dir.home()


def onTrigger(context, session):
  flow_file = session.get()

  if flow_file is not None:
    log.info('got flow file: %s' % flow_file.getAttribute('command'))
    data = ''
    data = flow_file.getAttribute('command')
    if data == ctrl_cmd[0]:
        motor.forward()
    elif data == ctrl_cmd[1]:
        motor.backward()
    elif data == ctrl_cmd[2]:
        car_dir.turn_left()
    elif data == ctrl_cmd[3]:
        car_dir.turn_right()
    elif data == ctrl_cmd[6]:
        car_dir.home()
    elif data == ctrl_cmd[4]:
        motor.ctrl(0)
    elif data == ctrl_cmd[5]:
        temp = cpu_temp.read()
        tcpCliSock.send('[%s] %0.2f' % (ctime(), temp))
    elif data == ctrl_cmd[8]:
        video_dir.move_increase_x()
    elif data == ctrl_cmd[9]:
        video_dir.move_decrease_x()
    elif data == ctrl_cmd[10]:
        video_dir.move_increase_y()
    elif data == ctrl_cmd[11]:
        video_dir.move_decrease_y()
    elif data == ctrl_cmd[12]:
        video_dir.home_x_y()
    elif data[0:5] == 'speed':     # Change the speed
        numLen = len(data) - len('speed')
        if numLen == 1 or numLen == 2 or numLen == 3:
            tmp = data[-numLen:]
            spd = int(tmp)
            if spd < 24:
                spd = 24
            motor.setSpeed(spd)
    elif data[0:5] == 'turn=':    #Turning Angle
        angle = data.split('=')[1]
        try:
            angle = int(angle)
            car_dir.turn(angle)
        except:
            print ('Error: angle =', angle)
    elif data[0:8] == 'forward=':
        spd = data[8:]
        try:
            spd = int(spd)
            motor.forward(spd)
        except:
            print ('Error speed =', spd)
    elif data[0:9] == 'backward=':
        spd = data.split('=')[1]
        try:
            spd = int(spd)
            motor.backward(spd)
        except:
            print ('ERROR, speed =', spd)

    else:
        print ('Command Error! Cannot recognize command: ' + data)
    session.transfer(flow_file, REL_SUCCESS)




