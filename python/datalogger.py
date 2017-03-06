import serial
import serial.tools.list_ports
import datetime

def findArduino():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if "ttyACM" in p[0] or "cu.usbmodem" in p[0]:
            print ("Using serial port: " + p[0])
            return str(p[0])

time = datetime.datetime.now().time().isoformat()
f = open(time + ".csv",'a')

while True:
    try:
        time = datetime.datetime.now().time().isoformat()
        f.write(time + "," + ser.readline())
        f.flush()
    except:
        time.sleep(0.1)
        port = findArduino()
        if port is not None:
            # reconnect
            try:
                print ("Connecting...")
                ser = serial.Serial(port, 115200)
                time = datetime.datetime.now().time().isoformat()
                f = open(time + ".csv",'a')
            except:
                print ("Connect failed.")
                f.close()
        else:
            print ("Connect failed.")
            f.close()