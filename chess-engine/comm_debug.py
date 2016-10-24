from comm import Comm

def debugReceive():
    Comm.setPort('COM5')
    ser = Comm.getSerial()
    print ser.name
    received = ''
    while not received:
        while ser.inWaiting() > 0:
            received += ser.read(13)
        if len(received) == 0:
            pass
            #print 'Nothing received.'
        else:
            print received

def debugSend():
    Comm.setPort('COM5')
    ser = Comm.getSerial()
    print ser.name
    ser.write('Hello world!')
    #ser.write('Hello world?')   # negative test
    print 'Done sending.'

if __name__ == '__main__':
    #debugReceive()
    debugSend()
