import smbus
import time

bus = smbus.SMBus(1)
address = 8
while True:
    try:
        bus.write_byte(address,42)
        time.sleep(1)
        n = bus.read_byte(address)
        print n
    except:
        print "Exception"
        continue
    time.sleep(1)
