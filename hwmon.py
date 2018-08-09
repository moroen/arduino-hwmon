#!/usr/bin/env python3

import json
import os

import psutil
import serial

sysinfo = dict()

def bytesto(bytes, to, bsize=1024):
    """convert bytes to megabytes, etc.
       sample code:
           print('mb= ' + str(bytesto(314575262000000, 'm')))
       sample output: 
           mb= 300002347.946
    """

    a = {'k' : 1, 'm': 2, 'g' : 3, 't' : 4, 'p' : 5, 'e' : 6 }
    r = float(bytes)
    for i in range(a[to]):
        r = r / bsize

    return(r)


# print (psutil.sensors_temperatures())
#print (psutil.sensors_fans())



# print ("{:2.2f}G/{:2.2f}G ({:2.2f}%)".format(bytesto(mem[3], 'g'), bytesto(mem[0], 'g'), mem[2]))

ser = serial.Serial("/dev/ttyACM0", baudrate=115200)
print (ser.name)

while 1:
    loadAvd = os.getloadavg()
    sysinfo["average_load"] = "{:2.2f} {:2.2f} {:2.2f}".format(loadAvd[0], loadAvd[1], loadAvd[2])

    mem = psutil.virtual_memory()
    sysinfo['mem'] = "{:2.2f} ({:2.1f}%)".format(bytesto(mem[3], 'g'), mem[2])

    cpu = psutil.cpu_percent(interval=1.0)

    if cpu != 0:
        sysinfo['cpu_percent'] = "{:5.1f}".format(cpu)
      
    ser.write(bytes(json.dumps(sysinfo)+"\n", 'utf-8'))


ser.close()
