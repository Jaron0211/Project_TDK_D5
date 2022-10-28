import  sensor, image, time , math
from pyb import UART
import pyb

usb = pyb.USB_VCP()

arduino = UART(3,9600,timeout_char = 1000)
arduino.init(9600 , bits = 8 , parity = None , stop = 1 , timeout_char = 1000)


thresholds = (220, 250)
min_degree = 0
max_degree = 180

DETECT_R = 5

sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.set_auto_whitebal(False,(-0.5,-1.2,-0.1))
sensor.set_auto_whitebal(False)

##color range
red = (55, 72, 20, 65, 14, 40) #(36, 77, 30, 55, 5, 54)
blue = (90, 55, -39, 20, -48, -6)
green = (95, 30, -66, -47, 11, 31)

light_filter = (80, 100, -120, 127, -120, 127)

clock = time.clock()

def two_circle_detection():
    img = sensor.snapshot()
    img.gaussian(0)

    all_little_circle = img.find_circles(threshold = 2700, x_margin = 40, y_margin = 40, r_margin = 20,
        r_min = 4, r_max = 40, r_step = 2)
    all_big_circle = img.find_circles(hreshold = 2000, x_margin = 30, y_margin = 30, r_margin = 40,
        r_min = 60, r_max = 100, r_step = 3)
    all_line = img.find_lines(threshold = 800, theta_margin = 10, rho_margin = 30)

    for C in all_big_circle:
        img.draw_circle(C.x(),C.y(),C.r(),color = (255,255,100))

    for C in all_little_circle:
        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] + 10**(-4))

            line_y_mid = (l.line()[3] - l.line()[1] + 10**(-4))

            A = line_y_mid / line_x_mid

            B = A*l.line()[0] - l.line()[1]

            apple203 = abs(A*C.x() -C.y() -B) / math.sqrt (A**2 + 1)

            if ( apple203 < DETECT_R):
                    img.draw_line((C.x(),C.y(),l.line()[0],l.line()[1]),color = (255,0,0))
                    print((C.x(),C.y(),l.line()[0],l.line()[1]))
            else:
                    img.draw_line(l.line(),color = (100,76,0))

        img.draw_circle(C.x(),C.y(),C.r(),color = (0,255,0))
    img.draw_string(80,60,"circle")

def find_blob():
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

    img.draw_string(80,60,"blob")



while(1):
    clock.tick()
    #C = arduino.readline()
    C = 2
    if(C == 1):
        two_circle_detection()
    elif(C == 2):
        find_blob()

