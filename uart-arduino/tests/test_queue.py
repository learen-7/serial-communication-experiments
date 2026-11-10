import serial, time

with serial.Serial('/dev/ttyACM0', 115200, timeout=1) as s:   
    time.sleep(2)       
    for _ in range(40):
        s.write(b'3\n')
    print(s.read(200))    