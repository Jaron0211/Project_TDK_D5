import  sensor, image, time , math
import pyb, ustruct


thresholds = (220, 250)
min_degree = 0
max_degree = 180

DETECT_R = 5

sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.set_auto_whitebal(False,(-0.5,-1.2,-0.1))

bus = pyb.I2C(2, pyb.I2C.SLAVE, addr=0x12)
bus.deinit() # Fully reset I2C device...
bus = pyb.I2C(2, pyb.I2C.SLAVE, addr=0x12)
print("Waiting for Arduino...")


##color range
red = (55, 72, 20, 65, 14, 40) #(36, 77, 30, 55, 5, 54)
blue = (90, 55, -39, 20, -48, -6)
green = (95, 30, -66, -47, 11, 31)

blue_sandbag = (90, 55, -39, 20, -48, -6)
green_sandbag = (95, 30, -66, -47, 11, 31)

light_filter = (80, 100, -120, 127, -120, 127)

clock = time.clock()

output_data = [0,0,0,0]

##basic function
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
    all_line = img.find_lines(threshold = 2500, theta_margin = 40, rho_margin = 30)
    img.draw_string(80,60,"LF",color=(0,0,0),size = 1)
    for l in all_line:
        img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

        line_x_mid = (l.line()[2] + l.line()[0])//2
        line_y_mid = (l.line()[3] + l.line()[1])//2

        x_revise = line_x_mid - 80
        y_revise = line_y_mid - 60

        try:
            line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
            line_ratio= math.degrees(line_ratio)
        except Exception as e:
            line_ratio = 90;
        c = str(line_ratio)
        img.draw_string(line_x_mid-20,line_y_mid,c,color=(0,0,0),size = 1)
        output_data[0] = 0
        output_data[1] = x_revise
        output_data[2] = y_revise
        output_data[3] = line_ratio

##combine function
def part1():
    img = sensor.snapshot()
    img.gaussian(0)

    all_little_circle = img.find_circles(threshold = 2700, x_margin = 40, y_margin = 40, r_margin = 20,
        r_min = 4, r_max = 40, r_step = 2)
    all_big_circle = img.find_circles(threshold = 2000, x_margin = 30, y_margin = 30, r_margin = 40,
        r_min = 60, r_max = 100, r_step = 3)
    all_line = img.find_lines(threshold = 2500, theta_margin = 40, rho_margin = 30)

    img.draw_string(80,60,"P1",color=(0,0,0),size = 1)

    for C in all_big_circle:
        img.draw_circle(C.x(),C.y(),C.r(),color = (255,255,100))
    for C in all_little_circle:
        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            try:
                A = (l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0])
            except Exception as e:
                A = 0
            B = A*l.line()[0] - l.line()[1]

            apple203 = abs(A*C.x() -C.y() -B) / math.sqrt (A**2 + 1)

            if ( apple203 < DETECT_R):
                img.draw_line((C.x(),C.y(),l.line()[0],l.line()[1]),color = (255,0,0))

                output_data[0] = 0
                output_data[1] = C.x() - 80
                output_data[2] = C.y() - 60
                output_data[3] = math.degrees(math.atan(A))

            else:
                img.draw_line(l.line(),color = (100,76,0))
        img.draw_circle(C.x(),C.y(),C.r(),color = (0,255,0))

def part2():
    img = sensor.snapshot()
    img.gaussian(0)

    COLOR = str("0")
    x_scale = 1
    y_scale = 1
    angle_scale = 1

    red_blobs = img.find_blobs([red],merge=True)
    all_line = img.find_lines(threshold = 2500, theta_margin = 40, rho_margin = 30)
    img.draw_string(80,60,"P2",color=(0,0,0),size = 1)

    for l in all_line:
        img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

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

    for a in red_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (255,0,0))

        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))

        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )

        for l in all_line:
            line_x_mid = (l.line()[2] - l.line()[0] )
            line_y_mid = (l.line()[3] - l.line()[1] )

            try:
                A = line_y_mid / line_x_mid
            except Exception as e:
                A = 0

            B = A * l.line()[0] - l.line()[1]
            apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))

            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))

                output_data[0] = 1

def part3():
    COLOR = str("0")
    X = str("0")
    Y = str("0")
    ANGLE = str("0")

    img = sensor.snapshot()#binary([light_filter], invert=False, zero=True)
    img.draw_string(80,60,"blob",color=(0,0,0),size = 1)

    red_blobs = img.find_blobs([red],merge=True)
    blue_blobs = img.find_blobs([blue],merge=True)
    green_blobs = img.find_blobs([green],merge=True)
    all_line = img.find_lines(threshold = 2500, theta_margin = 40, rho_margin = 30)

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

def part4():
    COLOR = str("0")
    X = str("0")
    Y = str("0")
    ANGLE = str("0")

    img = sensor.snapshot()#binary([light_filter], invert=False, zero=True)
    img.draw_string(80,60,"P4",color=(0,0,0),size = 1)

    red_blobs = img.find_blobs([red],merge=True)
    blue_blobs = img.find_blobs([blue],merge=True)
    green_blobs = img.find_blobs([green],merge=True)
    all_line = img.find_lines(threshold = 2500, theta_margin = 40, rho_margin = 30)

    for a in blue_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (0,0,255))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )

        output_data[0] = 2
        output_data[1] = x_mid - 80
        output_data[2] = y_mid - 60
        output_data[3] = 0


    for a in green_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (0,255,0))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )

        output_data[0] = 3
        output_data[1] = x_mid - 80
        output_data[2] = y_mid - 60
        output_data[3] = 0

    for a in red_blobs:
        if(a.rect()[2] * a.rect()[3] < 2500):
            continue
        img.draw_rectangle(a.rect(),color = (255,0,0))
        x_mid = (a.rect()[0] + (a.rect()[2]/2))
        y_mid = (a.rect()[1] + (a.rect()[3]/2))
        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )

        output_data[0] = 1
        output_data[1] = x_mid - 80
        output_data[2] = y_mid - 60
        output_data[3] = 0

def part5():
    img = sensor.snapshot()
    img.gaussian(0)
    x_scale = 1
    y_scale = 1
    angle_scale = 1
    all_line = img.find_lines(threshold = 2500, theta_margin = 40, rho_margin = 30)
    img.draw_string(80,60,"LF",color=(0,0,0),size = 1)
    for l in all_line:
        img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

        line_x_mid = (l.line()[2] + l.line()[0])//2
        line_y_mid = (l.line()[3] + l.line()[1])//2

        x_revise = line_x_mid - 80
        y_revise = line_y_mid - 60

        try:
            line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
            line_ratio= math.degrees(line_ratio)
        except Exception as e:
            line_ratio = 90;
        c = str(line_ratio)
        img.draw_string(line_x_mid-20,line_y_mid,c,color=(0,0,0),size = 1)
        output_data[0] = 0
        output_data[1] = x_revise
        output_data[2] = y_revise
        output_data[3] = line_ratio

def part6_1():
    all_line = img.find_lines(threshold = 800, theta_margin = 10, rho_margin = 30)
    if( len(all_line) <= 5):
        for L1 in all_line:
            for L2 in all_line:
                if( (L1.line()[0] - L1.line()[2])*(L2.line()[0] - L2.line()[2]) + (L1.line()[1] - L1.line()[3])*(L2.line()[1] - L2.line()[3]) > 0.85):
                    if( H_LINE == (0,0,0,0) & T_LINE == (0,0,0,0) ):

                       L1_RATIO = 0
                       L2_RATIO = 0
                       try:
                            line_x_mid = (L1.line()[2] + L1.line()[0])//2
                            line_y_mid = (L1.line()[3] + L1.line()[1])//2

                            x_revise = line_x_mid - 80
                            y_revise = line_y_mid - 60

                            line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
                            line_ratio = math.degrees(line_ratio)

                            L1_RATIO = line_ratio

                       except Exception as e:
                            line_ratio = 90

                            L1_RATIO = line_ratio

                       try:
                            line_x_mid = (L2.line()[2] + L2.line()[0])//2
                            line_y_mid = (L2.line()[3] + L2.line()[1])//2

                            x_revise = line_x_mid - 80
                            y_revise = line_y_mid - 60

                            line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
                            line_ratio= math.degrees(line_ratio)

                            L2_RATIO = line_ratio

                       except Exception as e:

                            line_ratio = 90
                            L2_RATIO = line_ratio

                       if(L2_RATIO > L1_RATIO):
                            T_LINE = L2.line()
                            H_LINE = L1.line()
                       elif(L2_RATIO > L1_RATIO):
                            T_LINE = L1.line()
                            H_LINE = L2.line()
                    else:

                       L1_RATIO = 0
                       L2_RATIO = 0

                       try:
                            line_x_mid = (L1.line()[2] + L1.line()[0])//2
                            line_y_mid = (L1.line()[3] + L1.line()[1])//2

                            x_revise = line_x_mid - 80
                            y_revise = line_y_mid - 60

                            line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
                            line_ratio = math.degrees(line_ratio)

                            L1_RATIO = line_ratio

                       except Exception as e:
                            line_ratio = 90

                            L1_RATIO = line_ratio

                       try:
                            line_x_mid = (L2.line()[2] + L2.line()[0])//2
                            line_y_mid = (L2.line()[3] + L2.line()[1])//2

                            x_revise = line_x_mid - 80
                            y_revise = line_y_mid - 60

                            line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
                            line_ratio= math.degrees(line_ratio)

                            L2_RATIO = line_ratio

                       except Exception as e:

                            line_ratio = 90
                            L2_RATIO = line_ratio

                       H_LINE_VECTOR = ((H_LINE[0] - H_LINE[2]),(H_LINE[1] - H_LINE[3]))
                       T_LINE_VECTOR = ((T_LINE[0] - T_LINE[2]),(T_LINE[1] - T_LINE[3]))

                else:
                   H_LINE = [(0,0),(0,0)]
                   T_LINE = [(0,0),(0,0)]
    else:
        H_LINE = [(0,0),(0,0)]
        T_LINE = [(0,0),(0,0)]

TANGENT_X = 0
TANGENT_Y = 0

def part6():
    img = sensor.snapshot()
    img.gaussian(0)

    all_line = img.find_lines(threshold = 2500, theta_margin = 40, rho_margin = 30)
    if( len(all_line) <= 10):
        for L1 in all_line:
            for L2 in all_line:
                A1 = (L1.line()[0] - L1.line()[2])
                A2 = (L1.line()[1] - L1.line()[3])
                B1 = (L2.line()[0] - L2.line()[2])
                B2 = (L2.line()[1] - L2.line()[3])

                CON1 = A1*B1 + A2*B2
                CON2 = (A1**2 + A2**2)**(1/2) * (B1**2 + B2**2)**(1/2)

                print(CON1/CON2)
                if( abs(CON1/CON2) < 0.2 ):

                   L1_RATIO = 0
                   L2_RATIO = 0
                   try:
                       line_x_mid = (L1.line()[2] + L1.line()[0])//2
                       line_y_mid = (L1.line()[3] + L1.line()[1])//2

                       x_revise = line_x_mid - 80
                       y_revise = line_y_mid - 60

                       line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
                       line_ratio = math.degrees(line_ratio)

                       L1_RATIO = line_ratio

                   except Exception as e:
                       line_ratio = 90

                       L1_RATIO = line_ratio

                   try:
                       line_x_mid = (L2.line()[2] + L2.line()[0])//2
                       line_y_mid = (L2.line()[3] + L2.line()[1])//2

                       x_revise = line_x_mid - 80
                       y_revise = line_y_mid - 60

                       line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
                       line_ratio= math.degrees(line_ratio)

                       L2_RATIO = line_ratio

                   except Exception as e:

                       line_ratio = 90
                       L2_RATIO = line_ratio

                   if(L2_RATIO > L1_RATIO):
                       T_LINE = L2.line()
                       H_LINE = L1.line()
                   elif(L2_RATIO > L1_RATIO):
                       T_LINE = L1.line()
                       H_LINE = L2.line()

                   X11 = L1.line()[0]
                   Y11 = L1.line()[1]
                   X12 = L1.line()[2]
                   Y12 = L1.line()[3]

                   X21 = L2.line()[0]
                   Y21 = L2.line()[1]
                   X22 = L2.line()[2]
                   Y22 = L2.line()[3]

                   a = -(X12 - X11)*(Y22 - Y21)*X21
                   b =  (X22 - X21)*(Y12 - Y11)*X11
                   c =  (X22 - X21)*(X12 - X11)*(Y21 - Y11)
                   d =  (Y12 - Y11)*(X22 - X21) - (Y22 - Y21)*(X12 - X11)

                   try:
                       POINT_X = (a+b+c)/d
                   except ZeroDivisionError as err:
                       POINT_X = 0

                   try:
                       POINT_Y = (Y12-Y11)/(X12 - X11) * (POINT_X - X11) + Y11
                   except ZeroDivisionError as err:
                       POINT_Y = 0

                   TANGENT_X = POINT_X - 80
                   TANGENT_Y = POINT_Y - 60
                   img.draw_cross(int(POINT_X) , int(POINT_Y), color = (255,255,0) , size = 1 )
                   img.draw_line(L1.line(),color = (0,255,0))
                   img.draw_line(L2.line(),color = (0,0,255))
                   if((TANGENT_X**2 + TANGENT_Y**2)**(1/2) < 10):
                        output_data[0] = 0
                        output_data[1] = 0
                   else:
                        output_data[0] = 5
                        output_data[1] = TANGENT_X

                   output_data[2] = TANGENT_Y
                   try:
                       output_data[3] = math.degrees(math.atan((Y12-Y11)/(X12 - X11)))
                   except ZeroDivisionError as err:
                       output_data[3] = 90
                else:
                   output_data[0] = 0
                   output_data[1] = 0
                   output_data[2] = 0
                   output_data[3] = 0
                   ##line_follow()

    else:
        pass
        ##line_follow()

def part7():
    img = sensor.snapshot()
    img.gaussian(0)

    COLOR = str("0")
    X = str("0")
    Y = str("0")
    ANGLE = str("0")

    line_x_point = 0
    line_y_point = 0
    line_detect_ratio = 0

    blue_blobs = img.find_blobs([blue_sandbag],merge=True)
    green_blobs = img.find_blobs([green_sandbag],merge=True)

    all_line = img.find_lines(threshold = 3500, theta_margin = 20, rho_margin = 50)
    img.draw_string(80,60,"P7",color=(0,0,0),size = 1)

    for l in all_line:
        img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

        line_x_mid = (l.line()[2] + l.line()[0])//2
        line_y_mid = (l.line()[3] + l.line()[1])//2

        x_revise = line_x_mid - 80
        y_revise = line_y_mid - 60

        try:
            line_ratio = math.atan((l.line()[3] - l.line()[1])/(l.line()[2] - l.line()[0]))
            line_ratio= math.degrees(line_ratio)

        except Exception as e:
            line_ratio = 90;

        c = str(line_ratio)
        img.draw_string(line_x_mid-20,line_y_mid,c,color=(0,0,0),size = 1)

        line_x_point = x_revise
        line_y_point = y_revise
        line_detect_ratio = line_ratio

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

MODE = 0
C_DECODE = 0

while(0):
    part5()

while(1):

    if(MODE == 1):
        part1()
    elif(MODE == 2):
        part2()
    elif(MODE == 3):
        part3()
    elif(MODE == 4):
        part4()
    elif(MODE == 5):
        part5()
    elif(MODE == 6):
        part6()
    elif(MODE == 7):
        part7()
    else:
        print("No mode select")

    send_data =  str(output_data[0]) + ";" + str(output_data[1]) + ";" + str(output_data[2]) + ";" + str(output_data[3])

    try:
        data = bus.recv(1,timeout = 200)
        print(data)

        if(data):
            print("ok")
            KE = ustruct.unpack('<b',data)
            MODE = KE[0]
            SENDING = ustruct.pack("<%ds" % len(send_data),send_data)
            print(MODE)
            print(len(send_data))
            try:
                bus.send(ustruct.pack("<h", len(send_data)))
                bus.send(send_data)
            except OSError as err:
                pass


    except OSError as err:
        pass
