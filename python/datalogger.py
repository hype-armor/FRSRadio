#!/usr/bin/env python
import serial
import serial.tools.list_ports
import datetime
import time
from threading import Lock
lock = Lock()

def findArduino():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if "ttyACM" in p[0] or "cu.usbmodem" in p[0]:
            print ("Using serial port: " + p[0])
            return str(p[0])

dt = datetime.datetime.now().time().isoformat()
f = open(dt + ".csv",'a')

try:
    while True:
        try:
            dt = datetime.datetime.now().time().isoformat()
            f.write(dt + "," + ser.readline())
            f.flush()
        except:
            time.sleep(1)
            port = findArduino()
            if port is not None:
                # reconnect
                try:
                    print ("Connecting...")
                    ser = serial.Serial(port, 115200)
                    dt = datetime.datetime.now().time().isoformat()
                    f = open(dt + ".csv",'a')
                except:
                    print ("Connect failed.")
                    f.close()
            else:
                print ("Connect failed.")
                f.close()
except KeyboardInterrupt:
    print "attempting to close threads."
    t.join(0)
    print "threads successfully closed"
