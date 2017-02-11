from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import SocketServer
import simplejson
import random
import serial
import os
import threading
import time
import serial.tools.list_ports
import mimetypes


def findArduino():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if "ttyACM" in p[0] or "cu.usbmodem" in p[0]:
            print ("Using serial port: " + p[0])
            return str(p[0])

def worker():
    print ("worker started")
    global serialData
    global ser

    serialData = ""
    while True:
        
        try:
            serialData = ser.readline()
        except:
            time.sleep(0.1)
            port = findArduino()
            if port is not None:
                try:
                    print ("Connecting...")
                    ser = serial.Serial(port, 115200)
                except:
                    print ("Connect failed.")
            else:
                rand = str(random.randrange(-350,2500))
                serialData = (str(random.randrange(0,3000)) + "," + str(random.randrange(0,3000)) + "," + str(random.randrange(0,3000)) + "," + str(random.randrange(0,3000)) + "," + str(random.randrange(0,3000)) + "," + 
                str(random.randrange(0,3000)) + "," + rand + "," + rand + "," + rand + "," + rand + "," + rand + "," + 
                rand + "," + rand + "," + rand + "," + rand + "," + rand + "," + rand + ",")
        
        #print ("Data=" + serialData)

class S(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    
    def do_GET(self):
        currentpath = os.getcwd()
        
        if self.path == "/data":
            self._set_headers()
            localData = serialData
            self.wfile.write(localData)
            return
        elif self.path == "/":
            self._set_headers()
            f = open("index.html", "r")
            self.wfile.write(f.read())
        elif "/css/" in self.path:
            self.send_response(200)
            self.send_header('Content-type', 'text/css')
            self.end_headers()
            f = open(currentpath+self.path, "r")
            self.wfile.write(f.read())
        elif "/js/" in self.path:
            self.send_response(200)
            self.send_header('Content-type', 'application/javascript')
            self.end_headers()
            f = open(currentpath+self.path, "r")
            self.wfile.write(f.read())
        else:
            #self._set_headers()
            mimetype = mimetypes.guess_type(currentpath+self.path)
            print (currentpath+self.path)
            self.send_response(200)
            self.send_header('Content-type', mimetype)
            self.end_headers()
            f = open(currentpath+self.path, "r")
            self.wfile.write(f.read())

    def do_HEAD(self):
        self._set_headers()

    def do_POST(self):
        self._set_headers()
        print ("in post method")
        self.data_string = self.rfile.read(int(self.headers['Content-Length']))

        self.send_response(200)
        self.end_headers()

        data = simplejson.loads(self.data_string)
        with open("test123456.json", "w") as outfile:
            simplejson.dump(data, outfile)
        print ("{}".format(data))
        f = open("for_presen.py")
        self.wfile.write(f.read())
        return

def run(server_class=HTTPServer, handler_class=S, port=8000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print ('Starting serial...')
    t = threading.Thread(target=worker)
    t.daemon = True
    t.start()
    print ('Starting httpd...')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print "attempting to close threads."
        t.join(0)
        print "threads successfully closed"
    
if __name__ == "__main__":
    from sys import argv

if len(argv) == 2:
    run(port=int(argv[1]))
else:
    run()