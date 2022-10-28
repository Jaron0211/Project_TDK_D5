import pyb, ustruct


# Use "ustruct" to build data packets to send.
# "<" puts the data in the struct in little endian order.
# "%ds" puts a string in the data stream. E.g. "13s" for "Hello World!\n" (13 chars).
# See https://docs.python.org/3/library/struct.html

bus = pyb.I2C(2, pyb.I2C.SLAVE, addr=0x12)
bus.deinit() # Fully reset I2C device...
bus = pyb.I2C(2, pyb.I2C.SLAVE, addr=0x12)
print("Waiting for Arduino...")

text = "hello print"

# Note that for sync up to work correctly the OpenMV Cam must be running this script before the
# Arduino starts to poll the OpenMV Cam for data. Otherwise the I2C byte framing gets messed up,
# and etc. So, keep the Arduino in reset until the OpenMV Cam is "Waiting for Arduino...".

while(True):
    i2c_read = ""
    if(MODE == 1):
    data = ustruct.pack("<%ds" % len(text), text)
    try:
        bus.recv( i2c_read, timeout=200 )

        try:
            if(i2c_read == "1"):
                bus.send(data, timeout=200)
                MODE = 1;
                print("Sent Data!")
            elif(i2c_read == "2"):
                bus.send(data, timeout=200)
                MODE = 2;
                print("Sent Data!")
            elif(i2c_read == "3"):
                bus.send(data, timeout=200)
                MODE = 3;
                print("Sent Data!")

        except OSError as err:
            pass

    except OSError as err:
        pass
