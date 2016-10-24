import serial

class Comm:
	_portname = ''
	_ser = None

	@staticmethod
	def getSerial():
		if Comm._ser is None:
			Comm._ser = serial.Serial(
				port=Comm._portname or '/dev/cu.usbserial-FTZ8NTHG',
				timeout=0.05,
				baudrate=9600,
				stopbits=serial.STOPBITS_ONE,
				bytesize=serial.EIGHTBITS
			)
		return Comm._ser

	@staticmethod
	def setPort(portname):
		Comm._portname = portname
