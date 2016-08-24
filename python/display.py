import serial
import datetime
from Tkinter import *

root = Tk()
afr= StringVar()
Label(root, text="AFR").pack()
label = Label(root, textvariable=afr)
label.pack()
ser = serial.Serial('/dev/ttyACM0', 115200)

afr.set("test")
afr.set("14.7")

def update():
	text = ser.readline()
	afr.set(text)
	label.after(100, update)
	
update()
root.mainloop()
