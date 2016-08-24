import serial
import datetime

ser = serial.Serial('/dev/ttyACM0', 115200)

text = ser.readline()
println(text)
