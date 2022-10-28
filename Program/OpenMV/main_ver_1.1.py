import  sensor, image, time , math
from pyb import UART
import pyb

arduino = UART(3,9600,timeout_char = 10)
arduino.init(9600, bits = 8 , parity = None , stop = 1 , timeout_char = 80,timeout = 10)


thresholds = (220, 250)
min_degree = 0
max_degree = 180

DETECT_R = 5

sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.set_auto_whitebal(False,(-0.5,-1.2,-0.1))


##color range
red = (55, 72, 20, 65, 14, 40) #(36, 77, 30, 55, 5, 54)
blue = (90, 55, -39, 20, -48, -6)
green = (95, 30, -66, -47, 11, 31)

light_filter = (80, 100, -120, 127, -120, 127)

clock = time.clock()

output_data = [0,0,0,0]

def two_circle_detection():
    X = str("0")
    Y = str("0")
    ANGLE = str("0")
    img = sensor.snapshot()
    img.gaussian(0)
    img.draw_string(80,60,"TCD",color=(0,0,0),size = 1)
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
                    line_output = (C.x(),C.y(),math.atan(l.line()[1]/l.line()[0]))
                    X = str(C.x())
                    Y = str(C.y())
                    ANGLE = str(math.atan(l.line()[1]-l.line()[3]/l.line()[0]-l.line()[2]))
                    output_data = [0,X,Y,ANGLE]
            else:
                    img.draw_line(l.line(),color = (100,76,0))
        img.draw_circle(C.x(),C.y(),C.r(),color = (0,255,0))

def find_blob():
    COLOR = str("0")
    X = str("0")
    Y = str("0")
    ANGLE = str("0")
    img = sensor.snapshot()#binary([light_filter], invert=False, zero=True)
    img.draw_string(80,60,"blob",color=(0,0,0),size = 1)
    red_blobs = img.find_blobs([red],merge=True)
    blue_blobs = img.find_blobs([blue],merge=True)
    green_blobs = img.find_blobs([green],merge=True)
    all_line = img.find_lines(threshold = 1000, theta_margin = 10, rho_margin = 30)
    for a in blue_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (0,0,255))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )
        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] + 10**(-4))
            line_y_mid = (l.line()[3] - l.line()[1] + 10**(-4))
            A = line_y_mid / line_x_mid
            B = A * l.line()[0] - l.line()[1]
            apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))
            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))
                print(l.line())
                COLOR = str(2)
                X = str(line_x_mid)
                Y = str(line_y_mid)
                ANGLE = str(A)
                output_data[0] = COLOR
                output_data[1] = X
                output_data[2] = Y
                output_data[3] = ANGLE
    for a in green_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (0,255,0))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )
        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] + 10**(-4))
            line_y_mid = (l.line()[3] - l.line()[1] + 10**(-4))
            A = line_y_mid / line_x_mid
            B = A * l.line()[0] - l.line()[1]
            apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))
            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))
                print(l.line())
                COLOR = 3
                X = line_x_mid
                Y = line_y_mid
                ANGLE = A
                output_data[0] = COLOR
                output_data[1] = X
                output_data[2] = Y
                output_data[3] = ANGLE
    for a in red_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (255,0,0))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )
        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] + 10**(-4) )
            line_y_mid = (l.line()[3] - l.line()[1] + 10**(-4) )
            A = line_y_mid / line_x_mid
            B = A * l.line()[0] - l.line()[1]
            apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))
            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))
                print(l.line())
                COLOR = 1
                X = line_x_mid
                Y = line_y_mid
                ANGLE = A
                output_data[0] = COLOR
                output_data[1] = X
                output_data[2] = Y
                output_data[3] = ANGLE

def line_follow():
    img = sensor.snapshot()
    img.gaussian(0)
    x_scale = 1
    y_scale = 1
    angle_scale = 1
    all_line = img.find_lines(threshold = 800, theta_margin = 10, rho_margin = 30)
    img.draw_string(80,60,"LF",color=(0,0,0),size = 1)
    for l in all_line:
        img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))
        ##arduino.print(char(l.line()[0])+char(l.line()[1])+char(l.line()[2])+char(l.line()[3])+";")
        line_x_mid = (l.line()[2] - l.line()[0] )
        line_y_mid = (l.line()[3] - l.line()[1] )
        x_revise = line_x_mid - 80
        y_revise = line_y_mid - 60
        try:
            line_ratio = math.atan(line_y_mid/line_x_mid)
        except Exception as e:
            line_ratio = 0;
        output_data[0] = 0
        output_data[1] = x_revise
        output_data[2] = y_revise
        output_data[3] = line_ratio

def part1():
    img = sensor.snapshot()
    img.gaussian(0)

    all_little_circle = img.find_circles(threshold = 2700, x_margin = 40, y_margin = 40, r_margin = 20,
        r_min = 4, r_max = 40, r_step = 2)
    all_big_circle = img.find_circles(threshold = 2000, x_margin = 30, y_margin = 30, r_margin = 40,
        r_min = 60, r_max = 100, r_step = 3)
    all_line = img.find_lines(threshold = 800, theta_margin = 10, rho_margin = 30)

    img.draw_string(80,60,"P1",color=(0,0,0),size = 1)

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
                rate = apple203/DETECT_R
                if (rate > output_data[3]):
                    output_data[0] = C.x()
                    output_data[1] = C.y()
                    output_data[2] = math.atan(A)
                    output_data[3] = rate
            else:
                img.draw_line(l.line(),color = (100,76,0))
        img.draw_circle(C.x(),C.y(),C.r(),color = (0,255,0))


MODE = 0
C_DECODE = 0

NUM1 = str(1)
NUM2 = str(2)
NUM3 = str(3)

while(1):
    if(arduino.any() != 0):

        C = arduino.read(arduino.any())
        try:
            C_DECODE = C.decode()
            print(type(C_DECODE))
        except Exception:
            pass
        print(C_DECODE)
        if(C_DECODE == NUM1 or C_DECODE == NUM2 or C_DECODE == NUM3 ):

            arduino.write(str(output_data[0]))
            arduino.write(str(";"))
            arduino.write(str(output_data[1]))
            arduino.write(str(";"))
            arduino.write(str(output_data[2]))
            arduino.write(str(";"))
            arduino.write(str(output_data[3]))
            arduino.write("\n")

    if(C_DECODE == NUM1):
        part1()
    elif(C_DECODE == NUM2):
        find_blob()
    elif(C_DECODE == NUM3):
        line_follow()
