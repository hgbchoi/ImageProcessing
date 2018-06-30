import serial
import time
import struct



ser = serial.Serial('/dev/ttyACM0',9600, timeout=1)
byteReceived = '0'
data = [0, 1, 0, 1, 1, 1, 1, 1, 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
while byteReceived != b'R':
     ser.flushInput()
     try:
        byteReceived = ser.read()
        byteReceived = byteReceived.strip()
            
     except:
        pass
byteReceived = '0'
ser.write(data);

#for i in range (0, 5) :
#    ser.write(chr(data[i]).encode('UTF-8'))
#print('wrote')