import serial
ser = serial.Serial('/dev/ttyUSB0', timeout=0.1)
c = ser.read()
str = ser.read(10)
line = ser.readline()
ser.close()
