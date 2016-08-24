import serial
import datetime

ser = serial.Serial('/dev/ttyACM0',115200)

f = open('dataFile.txt','a')

while 1 :
    time = datetime.datetime.now().time().isoformat()
    f.write(time + "," + ser.readline())
    f.close()
    f = open('dataFile.txt','a')
