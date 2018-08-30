#/usr/bin/python3.5 save_data.py
import time
import serial
import datetime

ser = None

def connect():
	global ser
	ser = serial.Serial(
		port='/dev/ttyUSB0',
		baudrate=500000,
		#baudrate=115200,
		#baudrate=9600,
		parity=serial.PARITY_NONE,
		stopbits=serial.STOPBITS_ONE,
		bytesize=serial.EIGHTBITS,
		timeout = 0.1
	)

	# ls /dev/cu.* <--- find device
	return ser.isOpen()

def close():
	ser.close()

if __name__ == "__main__":
	if not connect():
		print("Cannot connect to serial port")
		exit(1)
	#f = open("data.txt", "a")
	ser.write(b'.xy\n')
	ser.flush()
	while True:
		line = ser.readline()
		value = line.decode('latin1')[:-1]
		print(datetime.datetime.now(), " : ", value)
		#f.write(str(datetime.datetime.now()))
		#f.write(" : ")
		#f.write(value)
		#f.write('\n')
		#f.flush()

		if len(line)==0:
			time.sleep(1.)

	pass
