from comm import ser

def debugReceive():
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
    print ser.name
    ser.write('Hello world!')
    print 'Done sending.'

if __name__ == '__main__':
    #debugReceive()
    debugSend()
