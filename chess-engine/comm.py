import serial

# Parameters of device
ser = serial.Serial(
	port='/dev/cu.usbserial-FTZ8NTHG',
    timeout=0.05,
	baudrate=9600,
	stopbits=serial.STOPBITS_ONE,
	bytesize=serial.EIGHTBITS
)