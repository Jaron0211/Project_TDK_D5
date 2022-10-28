# Untitled - By: Pulag - Fri Apr 17 2020

import sensor, image, time , math
from pyb import UART

uart = UART(1,9600,timeout_char = 1000)
uart.init(9600 , bits = 8 , parity = None , stop = 1 , timeout_char = 1000)

arduino = UART(1,9600,timeout_char = 1000)
arduino.init(9600 , bits = 8 , parity = None , stop = 1 , timeout_char = 1000)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.set_auto_whitebal(False,(-0.5,-1.2,-0.1))

sensor.skip_frames(time = 2000)

##color range
red = (55, 72, 20, 65, 14, 40) #(36, 77, 30, 55, 5, 54)
blue = (90, 55, -39, 20, -48, -6)
green = (95, 30, -66, -47, 11, 31)

light_filter = (80, 100, -120, 127, -120, 127)


clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot()#.binary([light_filter], invert=False, zero=True)


    red_blobs = img.find_blobs([red],merge=True)
    blue_blobs = img.find_blobs([blue],merge=True)
    green_blobs = img.find_blobs([green],merge=True)
    all_line = img.find_lines(threshold = 1000, theta_margin = 10, rho_margin = 30)


    for a in red_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (255,0,0))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )
        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] + 10**(-4))
            line_y_mid = (l.line()[3] - l.line()[1])
            A = line_y_mid / line_x_mid
            B = A * l.line()[0] - l.line()[1]
            apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))

            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))
                print(l.line())


    for a in blue_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (0,0,255))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )
        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] + 10**(-4))
            line_y_mid = (l.line()[3] - l.line()[1])
            A = line_y_mid / line_x_mid
            B = A * l.line()[0] - l.line()[1]
            apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))

            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))
                print(l.line())


    for a in green_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (0,255,0))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )
        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] + 10**(-4))
            line_y_mid = (l.line()[3] - l.line()[1])
            A = line_y_mid / line_x_mid
            B = A * l.line()[0] - l.line()[1]
            apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))

            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))
                print(l.line())


    uart.write()
