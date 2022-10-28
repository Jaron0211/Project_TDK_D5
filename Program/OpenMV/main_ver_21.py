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

##mission record

##color range
red = (60, 80, 30, 65, 5, 50) #(36, 77, 30, 55, 5, 54)
blue = (60, 80, -20, 35, -60, -20)
green = (75, 95, -80, -40, 15, 43)

blue_sandbag = (10, 70, -20, 50, -80, -20)
green_sandbag = (15, 60, -60, -20, 0, 60)

black_circle = (0,20,-30,0,0,30)

red_line = ( 35, 55, 40, 80, 40, 80)

light_filter = (80, 100, -120, 127, -120, 127)

clock = time.clock()

output_data = [0,0,0,0]

##combine function

##take off area find circle and line
def part1():
    img = sensor.snapshot()

    all_little_circle = img.find_circles(threshold = 2500, x_margin = 20, y_margin = 20, r_margin = 20,
        r_min = 4, r_max = 40, r_step = 2)

    all_line = img.find_lines(threshold = 1200, theta_margin = 40, rho_margin = 30)

    img.draw_string(80,60,"P1",color=(0,0,0),size = 1)

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 0

    for C in all_little_circle:
        if (C.r() > 12):
            continue

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
                output_data[3] = l.theta() - 90

            else:

                img.draw_line(l.line(),color = (100,76,0))
                output_data[0] = 0
                output_data[1] = C.x() - 80
                output_data[2] = C.y() - 60
                output_data[3] = 0

        img.draw_circle(C.x(),C.y(),C.r(),color = (0,255,0))

##follow line to find the blob
def part2():

    img = sensor.snapshot()

    img.bilateral(1,0.3,0.3)

    COLOR = str("0")
    x_revise = 0
    y_revise = 0
    line_ratio = 0

    red_blobs = img.find_blobs([red],merge = 1,margin = 40)
    all_line = img.find_lines(threshold = 1500, theta_margin = 10, rho_margin = 10)
    img.draw_string(80,60,"P2",color=(0,0,0),size = 1)

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 0

    for l in all_line:
        if(abs(l.theta() - 90) > 45):

            img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

            line_x_mid = (l.line()[2] + l.line()[0] )//2
            line_y_mid = (l.line()[3] + l.line()[1] )//2

            COLOR = 0
            x_revise = line_x_mid - 80
            y_revise = line_y_mid - 60
            line_ratio = l.theta()- 90

    for a in red_blobs:

        if (a.area() < 700):
            continue
        if(a.cy() < 55):
            continue

        img.draw_edges(a.min_corners(), color=(255,0,0))

        x_mid = a.cx()
        y_mid = a.cy()

        img.draw_cross(int(x_mid) , int(y_mid), color = (255,255,0) , size = 1 )
        COLOR = 1

        for l in all_line:
            if(abs(l.theta() - 90) > 45):
                line_x_mid = (l.line()[2] + l.line()[0] )//2
                line_y_mid = (l.line()[3] + l.line()[1] )//2


                det_x = l.line()[2] - l.line()[0]
                det_y = l.line()[3] - l.line()[1]

                try:
                    A = det_y / det_x
                except Exception as e:
                    A = 0

                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * x_mid) - y_mid- B) / math.sqrt((A*A + (1)**2))

                if(apple203 < 10):
                    img.draw_line(l.line(),color = (250,250,0))


                    x_revise = line_x_mid - 80
                    y_revise = line_y_mid - 60
                    line_ratio = l.theta()- 90

                else:
                    img.draw_line(l.line(), color = (100,0,100))


    output_data[0] = COLOR
    output_data[1] = x_revise
    output_data[2] = y_revise
    output_data[3] = line_ratio

    print(str(x_revise)+" "+str(line_ratio))

##waiting red to change to blue/green
def part3():
    COLOR = str("0")
    X = str("0")
    Y = str("0")
    ANGLE = 90

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 90

    img = sensor.snapshot()

    img.bilateral(1,0.2,0.2)

    img.draw_string(80,60,"blob",color=(0,0,0),size = 1)

    red_blobs = img.find_blobs([red],merge=True)
    blue_blobs = img.find_blobs([blue],merge=True)
    green_blobs = img.find_blobs([green],merge=True)

    all_line = img.find_lines(threshold = 1500, theta_margin = 10, rho_margin = 10)
    all_little_circle = img.find_circles(threshold = 3000, x_margin = 20, y_margin = 20, r_margin = 20,
        r_min = 4, r_max = 40, r_step = 2)

    X_AREA = int(15)
    Y_AREA = int(12)
    img.draw_edges([(80-X_AREA,60+Y_AREA),(80-X_AREA,60-Y_AREA),(80+X_AREA,60-Y_AREA),(80+X_AREA,60+Y_AREA)], color=(255,0,0))

    #FIND BLUE AREA
    for a in blue_blobs:
        if (a.area() < 800):
             continue
        img.draw_edges(a.min_corners(), color=(255,0,0))

        COLOR = 2
        x_mid = a.cx()
        y_mid = a.cy()

        X = a.cx() - 80
        Y = a.cy() - 60

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            #X = line_x_mid - 80
            #Y = line_y_mid - 60

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * x_mid) - y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))

                X = line_x_mid - 80
                #Y = line_y_mid - 60
                ANGLE = angle_got

    for a in green_blobs:
        if (a.area() < 800):
             continue
        img.draw_edges(a.min_corners(), color=(255,0,0))

        COLOR = 3
        x_mid = a.cx()
        y_mid = a.cy()

        X = a.cx() - 80
        Y = a.cy() - 60

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            #X = line_x_mid - 80
            #Y = line_y_mid - 60

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * x_mid) - y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))

                X = line_x_mid - 80
                #Y = line_y_mid - 60
                ANGLE = angle_got


    for a in red_blobs:
        if (a.area() < 800):
             continue
        img.draw_edges(a.min_corners(), color=(255,0,0))
        COLOR = 1
        x_mid = a.cx()
        y_mid = a.cy()

        X = a.cx() - 80
        Y = a.cy() - 60

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            #X = line_x_mid - 80
            #Y = line_y_mid - 60

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * x_mid) - y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 5):
                img.draw_line(l.line(),color = (250,250,0))

                X = line_x_mid - 80
                #Y = line_y_mid - 60
                ANGLE = angle_got

    #for c in all_little_circle:
    #    if (c.r() > 8):
    #        X = c.x() - 80
    #        Y = c.y() - 60
    #        img.draw_circle(c.x(),c.y(),c.r(),color = (255,255,255),thickness = 2)

    output_data[0] = COLOR
    output_data[1] = X
    output_data[2] = Y
    output_data[3] = ANGLE

    img.draw_cross(int(X)+80 , int(Y)+60, color = (255,255,0) , size = 5 )
    print(str(output_data[0])+" "+str(output_data[1])+" "+str(output_data[2])+" "+str(output_data[3]))

##horizon fly to tangent point
def part4():
    img = sensor.snapshot()
    img.gaussian(0)

    blue_blobs = img.find_blobs([blue],merge=True)
    green_blobs = img.find_blobs([green],merge=True)

    stop_line = img.find_blobs([red_line],merge = 1)
    all_line = img.find_lines(threshold = 1500, theta_margin = 10, rho_margin = 10)

    detect_status = 0
    red_line_detect = 0

    x_revise = 0
    y_revise = 0
    line_ratio = 0

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 0

    img.draw_string(80,60,"P4",color=(0,0,0),size = 1)

    blob_detection = 0

    X_AREA = int(18)
    Y_AREA = int(15)
    img.draw_edges([(80-X_AREA,60+Y_AREA),(80-X_AREA,60-Y_AREA),(80+X_AREA,60-Y_AREA),(80+X_AREA,60+Y_AREA)], color=(255,0,0))

    for a in stop_line:
        if(a.area()< 300 and a.area()>1000):
            pass
        img.draw_edges(a.min_corners(), color=(255,255,255))
        red_line_detect = 1

    for a in blue_blobs:
        if (a.area() < 1500):
             continue
        img.draw_edges(a.min_corners(), color=(0,0,255))
        blob_detection = 1
        x_mid = a.cx()
        y_mid = a.cy()

        x_revise = a.cx() - 80
        y_revise = a.cy() - 60


    for a in green_blobs:
        if (a.area() < 1500):
             continue
        img.draw_edges(a.min_corners(), color=(0,255,0))
        blob_detection = 1
        x_mid = a.cx()
        y_mid = a.cy()

        x_revise = a.cx() - 80
        y_revise = a.cy() - 60


    if( len(all_line) <= 5 and blob_detection == 0):

        if(red_line_detect == 0):
            for L1 in all_line:
                for L2 in all_line:

                    A1 = (L1.line()[0] - L1.line()[2])
                    A2 = (L1.line()[1] - L1.line()[3])
                    B1 = (L2.line()[0] - L2.line()[2])
                    B2 = (L2.line()[1] - L2.line()[3])

                    CON1 = A1*B1 + A2*B2
                    CON2 = (A1**2 + A2**2)**(1/2) * (B1**2 + B2**2)**(1/2)
                    try:
                        CON1/CON2
                    except ZeroDivisionError as e:
                        continue

                    if( abs(CON1/CON2) < 0.4 ):

                       L1_RATIO = 0
                       L2_RATIO = 0

                       L1_RATIO = L1.theta()
                       L2_RATIO = L2.theta()

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
                           POINT_X = 80

                       try:
                           POINT_Y = (Y12-Y11)/(X12 - X11) * (POINT_X - X11) + Y11
                       except ZeroDivisionError as err:
                           POINT_Y = 60

                       TANGENT_X = POINT_X - 80
                       TANGENT_Y = POINT_Y - 60

                       if((POINT_X <= 150 and POINT_X >= 10)and(POINT_Y <= 115 and POINT_Y >= 5)):
                           detect_status = 5
                           x_revise = TANGENT_X
                           y_revise = TANGENT_Y
                           match_ratio = str(abs(CON1/CON2))
                           img.draw_string(int(TANGENT_X)+70,int(TANGENT_Y)+50,match_ratio,color=(0,0,0),size = 1)

                           if(abs(L2_RATIO - 90) > abs(L1_RATIO - 90)):
                               line_ratio = L1.theta() - 90
                               img.draw_line(L1.line(),color = (0,255,0))
                               img.draw_line(L2.line(),color = (0,0,255))
                           elif(abs(L2_RATIO - 90) < abs(L1_RATIO - 90)):
                               line_ratio = L2.theta() - 90
                               img.draw_line(L2.line(),color = (0,255,0))
                               img.draw_line(L1.line(),color = (0,0,255))
        if(detect_status != 5):
            for l in all_line:
               img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

               line_x_mid = (l.line()[2] + l.line()[0])//2
               line_y_mid = (l.line()[3] + l.line()[1])//2

               if(abs(l.theta() - 90) < 46):
                   detect_status = 0
                   x_revise = line_x_mid - 80
                   y_revise = line_y_mid - 60
                   line_ratio = l.theta() - 90

                   c = str(line_ratio)
                   img.draw_string(line_x_mid-20,line_y_mid,c,color=(0,0,0),size = 1)

    x_revise = int(x_revise)
    y_revise = int(y_revise)

    output_data[0] = detect_status
    output_data[1] = x_revise
    output_data[2] = y_revise
    output_data[3] = line_ratio

    img.draw_cross(int(x_revise) + 80 , int(y_revise) + 60, color = (255,255,0) , size = 3 )
    print(str(detect_status)+" "+str(x_revise)+" "+str(y_revise)+" "+str(line_ratio))

##line following
def part5():
    img = sensor.snapshot()
    img.gaussian(0)

    x_revise = 0
    y_revise = 0
    line_ratio = 90

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 0

    all_line = img.find_lines(threshold = 1400, theta_margin = 40, rho_margin = 30)
    img.draw_string(80,60,"P5",color=(0,0,0),size = 1)

    for l in all_line:

        line_x_mid = (l.line()[2] + l.line()[0])//2
        line_y_mid = (l.line()[3] + l.line()[1])//2
        if(abs(l.theta()-90) > 46):
            img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))
            x_revise = line_x_mid - 80
            y_revise = line_y_mid - 60
            line_ratio = l.theta()-90

            c = str(line_ratio)
            img.draw_string(line_x_mid-20,line_y_mid,c,color=(0,0,0),size = 1)
            img.draw_line(l.line(),color = (255,0,0))

    output_data[0] = 0
    output_data[1] = x_revise
    output_data[2] = y_revise
    output_data[3] = line_ratio

##find tangent point
def part6():
    img = sensor.snapshot()
    img.gaussian(0)

    all_line = img.find_lines(threshold = 1600, theta_margin = 10, rho_margin = 10)

    detect_status = 0
    x_revise = 0
    y_revise = 0
    line_ratio = 90

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 90

    img.draw_string(80,60,"P6",color=(0,0,0),size = 1)

    blue_blobs = img.find_blobs([blue_sandbag],merge=True,margin = 10)
    green_blobs = img.find_blobs([green_sandbag],merge=True,margin = 10)

    green_x_mid = 0
    green_y_mid = 0

    blue_x_mid = 0
    blue_y_mid = 0

    blob_detection = 0

    for a in blue_blobs:
        if (a.area() < 100):
            continue

        img.draw_edges(a.min_corners(), color=(0,0,255))
        img.draw_cross(int(a.cx()) , int(a.cy()), color = (255,0,0) , size = 1 )

        blue_x_mid = a.cx()
        blue_y_mid = a.cy()

        x_revise = blue_x_mid - 80
        y_revise = blue_y_mid - 60
        blob_detection = 1

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * blue_x_mid) - blue_y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 10):
                img.draw_line(l.line(),color = (250,250,0))

                x_revise = line_x_mid - 80
                #Y = line_y_mid - 60
                line_ratio = angle_got

    for a in green_blobs:
        if (a.area() < 100):
            continue

        img.draw_edges(a.min_corners(), color=(0,255,0))
        img.draw_cross(int(a.cx()) , int(a.cy()), color = (255,0,0) , size = 1 )

        green_x_mid = a.cx()
        green_y_mid = a.cy()

        x_revise = green_x_mid - 80
        y_revise = green_y_mid - 60
        blob_detection = 1

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * green_x_mid) - green_y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 10):
                img.draw_line(l.line(),color = (250,250,0))

                x_revise = line_x_mid - 80
                #Y = line_y_mid - 60
                line_ratio = angle_got


    if( len(all_line) <= 10 and blob_detection == 0):
        for l in all_line:
            img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            if(abs(l.theta() - 90) > 46):
                detect_status = 0
                x_revise = line_x_mid - 80
                y_revise = line_y_mid - 60
                line_ratio = l.theta() - 90

                c = str(line_ratio)
                img.draw_string(line_x_mid-20,line_y_mid,c,color=(0,0,0),size = 1)

        for L1 in all_line:
            for L2 in all_line:

                A1 = (L1.line()[0] - L1.line()[2])
                A2 = (L1.line()[1] - L1.line()[3])
                B1 = (L2.line()[0] - L2.line()[2])
                B2 = (L2.line()[1] - L2.line()[3])

                CON1 = A1*B1 + A2*B2
                CON2 = (A1**2 + A2**2)**(1/2) * (B1**2 + B2**2)**(1/2)

                try:
                    CON1/CON2
                except ZeroDivisionError as e :
                    continue

                if( abs(CON1/CON2) < 0.4 ):

                   L1_RATIO = 0
                   L2_RATIO = 0

                   L1_RATIO = L1.theta()
                   L2_RATIO = L2.theta()

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
                       POINT_X = 80

                   try:
                       POINT_Y = (Y12-Y11)/(X12 - X11) * (POINT_X - X11) + Y11
                   except ZeroDivisionError as err:
                       POINT_Y = 60

                   TANGENT_X = POINT_X - 80
                   TANGENT_Y = POINT_Y - 60

                   if((POINT_X <= 160 and POINT_X >= 0)and(POINT_Y <= 120 and POINT_X >= 0)):
                       detect_status = 5
                       x_revise = TANGENT_X
                       y_revise = TANGENT_Y

                       if(abs(L2_RATIO - 90) > abs(L1_RATIO - 90)):
                           line_ratio = L2.theta() - 90
                           img.draw_line(L1.line(),color = (0,255,0))
                           img.draw_line(L2.line(),color = (0,0,255))
                       elif(abs(L2_RATIO - 90) < abs(L1_RATIO - 90)):
                           line_ratio = L1.theta() - 90
                           img.draw_line(L2.line(),color = (0,255,0))
                           img.draw_line(L1.line(),color = (0,0,255))

    x_revise = int(x_revise)
    y_revise = int(y_revise)

    output_data[0] = detect_status
    output_data[1] = x_revise
    output_data[2] = y_revise
    output_data[3] = line_ratio

    img.draw_cross(int(x_revise) + 80 , int(y_revise) + 60, color = (255,255,0) , size = 3 )
    print(str(detect_status)+" "+str(x_revise)+" "+str(y_revise)+" "+str(line_ratio))

def part72():
    img = sensor.snapshot()

    COLOR = 0
    X = 0
    Y = 0
    ANGLE = 90

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 0

    blob_detected = 0

    blue_blobs = img.find_blobs([blue_sandbag],merge=True,margin = 20)
    green_blobs = img.find_blobs([green_sandbag],merge=True,margin = 20)

    green_x_mid = 0
    green_y_mid = 0

    blue_x_mid = 0
    blue_y_mid = 0

    all_line = img.find_lines(threshold = 2000, theta_margin = 10, rho_margin = 20)
    img.draw_string(80,60,"P72",color=(0,0,0),size = 1)

    for a in green_blobs:
        if (a.area() < 2000):
            continue

        img.draw_edges(a.min_corners(), color=(0,255,0))
        img.draw_cross(int(a.cx()) , int(a.cy()), color = (255,0,0) , size = 1 )

        green_x_mid = a.cx()
        green_y_mid = a.cy()

        X = green_x_mid - 80
        Y = green_y_mid - 60

        blob_detected = 1

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            #X = line_x_mid - 80
            #Y = line_y_mid - 60

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * green_x_mid) - green_y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 10):
                img.draw_line(l.line(),color = (250,250,0))

                X = line_x_mid - 80
                #Y = line_y_mid - 60
                ANGLE = angle_got


    for a in blue_blobs:
        if (a.area() < 2000):
            continue

        if(a.cy() > 55):
            COLOR = 5
            img.draw_edges(a.min_corners(), color=(255,255,0))
        else:
            COLOR = 2
            img.draw_edges(a.min_corners(), color=(0,0,255))

        img.draw_cross(int(a.cx()) , int(a.cy()), color = (255,0,0) , size = 1 )

        blue_x_mid = a.cx()
        blue_y_mid = a.cy()

        X = blue_x_mid - 80
        Y = blue_y_mid - 60

        blob_detected = 1

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            #X = line_x_mid - 80
            #Y = line_y_mid - 60

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * blue_x_mid) - blue_y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 10):
                img.draw_line(l.line(),color = (250,250,0))

                X = line_x_mid - 80
                #Y = line_y_mid - 60
                ANGLE = angle_got

    if(blob_detected == 0 ):
        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            if(abs(l.theta() - 90) > 46):
                img.draw_line(l.line(), color = (255,0,0))
                x_revise = line_x_mid - 80
                y_revise = line_y_mid - 60
                line_ratio = l.theta() - 90

                c = str(line_ratio)
                img.draw_string(line_x_mid,line_y_mid,c,color=(0,0,0),size = 1)

                X = x_revise
                Y = y_revise
                ANGLE = line_ratio
                COLOR = 0

    output_data[0] = COLOR
    output_data[1] = X
    output_data[2] = Y
    output_data[3] = ANGLE

    img.draw_cross(int(X) + 80 , int(Y) + 60, color = (255,255,0) , size = 3 )
    print(str(output_data[0]) + " " + str(output_data[1]) + " " + str(output_data[2]) + " " + str(output_data[3]))

def part73():
    img = sensor.snapshot()

    COLOR = 0
    X = 0
    Y = 0
    ANGLE = 90

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 0

    blue_blobs = img.find_blobs([blue_sandbag],merge=True,margin = 100)
    green_blobs = img.find_blobs([green_sandbag],merge=True,margin = 100)

    green_x_mid = 0
    green_y_mid = 0

    blue_x_mid = 0
    blue_y_mid = 0

    all_line = img.find_lines(threshold = 2000, theta_margin = 10, rho_margin = 20)
    img.draw_string(80,60,"P72",color=(0,0,0),size = 1)

    for l in all_line:

        line_x_mid = (l.line()[2] + l.line()[0])//2
        line_y_mid = (l.line()[3] + l.line()[1])//2

        if(abs(l.theta() - 90) > 46):
            img.draw_line(l.line(), color = (255,0,0))
            x_revise = line_x_mid - 80
            y_revise = line_y_mid - 60
            line_ratio = l.theta() - 90

            c = str(line_ratio)
            img.draw_string(line_x_mid,line_y_mid,c,color=(0,0,0),size = 1)

            X = x_revise
            Y = y_revise
            ANGLE = line_ratio
            COLOR = 0

    for a in blue_blobs:
        if (a.area() < 2000):
            continue

        img.draw_edges(a.min_corners(), color=(0,0,255))
        img.draw_cross(int(a.cx()) , int(a.cy()), color = (255,0,0) , size = 1 )

        blue_x_mid = a.cx()
        blue_y_mid = a.cy()

        X = blue_x_mid - 80
        Y = blue_y_mid - 60

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            #X = line_x_mid - 80
            #Y = line_y_mid - 60

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * blue_x_mid) - blue_y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 10):
                img.draw_line(l.line(),color = (250,250,0))

                X = line_x_mid - 80
                #Y = line_y_mid - 60
                ANGLE = angle_got


    for a in green_blobs:
        if (a.area() < 2000):
            continue
        if(a.cy() > 55):
            COLOR = 5
            img.draw_edges(a.min_corners(), color=(255,255,0))
        else:
            COLOR = 2
            img.draw_edges(a.min_corners(), color=(0,255,0))

        img.draw_cross(int(a.cx()) , int(a.cy()), color = (255,0,0) , size = 1 )

        green_x_mid = a.cx()
        green_y_mid = a.cy()

        X = green_x_mid - 80
        Y = green_y_mid - 60

        for l in all_line:

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            #X = line_x_mid - 80
            #Y = line_y_mid - 60

            angle_got = l.theta() - 90

            det_x = (l.line()[2] - l.line()[0])
            det_y = (l.line()[3] - l.line()[1])

            if(abs(angle_got) < 46):
                 continue

            try:
                A = det_y / det_x
                B = A * l.line()[0] - l.line()[1]
                apple203 = abs((A * green_x_mid) - green_y_mid - B) / math.sqrt((A*A + (1)**2))

            except ZeroDivisionError as e:
                apple203 = 100

            if(apple203 < 10):
                img.draw_line(l.line(),color = (250,250,0))

                X = line_x_mid - 80
                #Y = line_y_mid - 60
                ANGLE = angle_got

    output_data[0] = COLOR
    output_data[1] = X
    output_data[2] = Y
    output_data[3] = ANGLE

##horizon fly
def part8():
    img = sensor.snapshot()
    img.gaussian(0)

    all_line = img.find_lines(threshold = 1500, theta_margin = 10, rho_margin = 10)

    detect_status = 0

    x_revise = 0
    y_revise = 0
    line_ratio = 0

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 0

    img.draw_string(80,60,"P8",color=(0,0,0),size = 1)

    X_AREA = int(18)
    Y_AREA = int(15)
    img.draw_edges([(80-X_AREA,60+Y_AREA),(80-X_AREA,60-Y_AREA),(80+X_AREA,60-Y_AREA),(80+X_AREA,60+Y_AREA)], color=(255,0,0))

    if( len(all_line) <= 5 ):

        for L1 in all_line:
            for L2 in all_line:

                A1 = (L1.line()[0] - L1.line()[2])
                A2 = (L1.line()[1] - L1.line()[3])
                B1 = (L2.line()[0] - L2.line()[2])
                B2 = (L2.line()[1] - L2.line()[3])

                CON1 = A1*B1 + A2*B2
                CON2 = (A1**2 + A2**2)**(1/2) * (B1**2 + B2**2)**(1/2)
                try:
                    CON1/CON2
                except ZeroDivisionError as e:
                    continue

                if( abs(CON1/CON2) < 0.4 ):

                   L1_RATIO = 0
                   L2_RATIO = 0

                   L1_RATIO = L1.theta()
                   L2_RATIO = L2.theta()

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
                       POINT_X = 80

                   try:
                       POINT_Y = (Y12-Y11)/(X12 - X11) * (POINT_X - X11) + Y11
                   except ZeroDivisionError as err:
                       POINT_Y = 60

                   TANGENT_X = POINT_X - 80
                   TANGENT_Y = POINT_Y - 60

                   if((POINT_X <= 150 and POINT_X >= 10)and(POINT_Y <= 115 and POINT_Y >= 5)):
                       detect_status = 5
                       x_revise = TANGENT_X
                       y_revise = TANGENT_Y
                       match_ratio = str(abs(CON1/CON2))
                       img.draw_string(int(TANGENT_X)+70,int(TANGENT_Y)+50,match_ratio,color=(0,0,0),size = 1)

                       if(abs(L2_RATIO - 90) > abs(L1_RATIO - 90)):
                           line_ratio = L1.theta() - 90
                           img.draw_line(L1.line(),color = (0,255,0))
                           img.draw_line(L2.line(),color = (0,0,255))
                       elif(abs(L2_RATIO - 90) < abs(L1_RATIO - 90)):
                           line_ratio = L2.theta() - 90
                           img.draw_line(L2.line(),color = (0,255,0))
                           img.draw_line(L1.line(),color = (0,0,255))

    if(detect_status != 5):
        for l in all_line:
           img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

           if(abs(l.theta() - 90) < 46):

               detect_status = 0

               line_x_mid = (l.line()[2] + l.line()[0])//2
               line_y_mid = (l.line()[3] + l.line()[1])//2
               x_revise = line_x_mid - 80
               y_revise = line_y_mid - 60
               line_ratio = l.theta() - 90

               c = str(line_ratio)
               img.draw_string(line_x_mid-20,line_y_mid,c,color=(0,0,0),size = 1)

    x_revise = int(x_revise)
    y_revise = int(y_revise)

    output_data[0] = detect_status
    output_data[1] = x_revise
    output_data[2] = y_revise
    output_data[3] = line_ratio

    img.draw_cross(int(x_revise) + 80 , int(y_revise) + 60, color = (255,255,0) , size = 3 )
    print(str(detect_status)+" "+str(x_revise)+" "+str(y_revise)+" "+str(line_ratio))

def part9():
    img = sensor.snapshot()

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 90

    detect_status = 0
    X = 0
    Y = 0
    ANGLE = 90

    all_line = img.find_lines(threshold = 1500, theta_margin = 10, rho_margin = 20)
    stop_line = img.find_blobs([red_line],merge = 1)

    img.draw_string(80,60,"P9",color=(0,0,0),size = 1)

    for l in all_line:

        if(abs(l.theta()-90) < 46):
             continue

        img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))
        line_x_mid = (l.line()[2] + l.line()[0])//2
        line_y_mid = (l.line()[3] + l.line()[1])//2

        x_revise = line_x_mid - 80
        y_revise = line_y_mid - 60

        line_ratio = l.theta() - 90

        c = str(line_ratio)
        img.draw_string(line_x_mid,line_y_mid,c,color=(0,0,0),size = 1)

        line_x_point = x_revise
        line_y_point = y_revise
        line_detect_ratio = line_ratio

    for a in stop_line:
        if(a.area() < 500 and a.area() > 1000):
            continue
        img.draw_edges(a.min_corners(), color=(255,255,255))
        detect_status = 1

    output_data[0] = detect_status
    output_data[1] = X
    output_data[2] = Y
    output_data[3] = ANGLE

    img.draw_cross(X,Y,color = (255,255,0),size = 1)
    print(str(detect_status)+" "+str(X)+" "+str(Y)+" "+str(ANGLE))


##detect red line tangent point
def part10():
    img = sensor.snapshot()

    detect_status = 0
    x_revise = 0
    y_revise = 0
    line_ratio = 90

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 90

    stop_line = img.find_blobs([red_line],merge = 1)
    all_line = img.find_lines(threshold = 1500, theta_margin = 10, rho_margin = 20)

    for a in stop_line:
        if(a.area()< 300 and a.area()>1000):
            pass
        img.draw_edges(a.min_corners(), color=(255,255,255))
        red_line_detect = 1
        y_revise = a.cy() - 60

    if( len(all_line) <= 10 ):
        for l in all_line:
            img.draw_line(l.line()[2],l.line()[3],l.line()[0],l.line()[1], color = (255,0,0))

            line_x_mid = (l.line()[2] + l.line()[0])//2
            line_y_mid = (l.line()[3] + l.line()[1])//2

            if(abs(l.theta() - 90) > 46):
                detect_status = 0
                x_revise = line_x_mid - 80
                #y_revise = line_y_mid - 60
                line_ratio = l.theta() - 90

                c = str(line_ratio)
                img.draw_string(line_x_mid-20,line_y_mid,c,color=(0,0,0),size = 1)

        for L1 in all_line:
            for L2 in all_line:

                A1 = (L1.line()[0] - L1.line()[2])
                A2 = (L1.line()[1] - L1.line()[3])
                B1 = (L2.line()[0] - L2.line()[2])
                B2 = (L2.line()[1] - L2.line()[3])

                CON1 = A1*B1 + A2*B2
                CON2 = (A1**2 + A2**2)**(1/2) * (B1**2 + B2**2)**(1/2)
                try:
                    CON1/CON2
                except ZeroDivisionError as e:
                    continue

                if( abs(CON1/CON2) < 0.4 ):

                   L1_RATIO = 0
                   L2_RATIO = 0

                   L1_RATIO = L1.theta()
                   L2_RATIO = L2.theta()

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
                       POINT_X = 80

                   try:
                       POINT_Y = (Y12-Y11)/(X12 - X11) * (POINT_X - X11) + Y11
                   except ZeroDivisionError as err:
                       POINT_Y = 60

                   TANGENT_X = POINT_X - 80
                   TANGENT_Y = POINT_Y - 60

                   if((POINT_X <= 160 and POINT_X >= 0)and(POINT_Y <= 120 and POINT_X >= 0)):
                       detect_status = 5
                       x_revise = TANGENT_X
                       y_revise = TANGENT_Y

                       if(abs(L2_RATIO - 90) > abs(L1_RATIO - 90)):
                           line_ratio = L2.theta() - 90
                           img.draw_line(L1.line(),color = (0,255,0))
                           img.draw_line(L2.line(),color = (0,0,255))
                       elif(abs(L2_RATIO - 90) < abs(L1_RATIO - 90)):
                           line_ratio = L1.theta() - 90
                           img.draw_line(L2.line(),color = (0,255,0))
                           img.draw_line(L1.line(),color = (0,0,255))

    x_revise = int(x_revise)
    y_revise = int(y_revise)

    output_data[0] = detect_status
    output_data[1] = x_revise
    output_data[2] = y_revise
    output_data[3] = line_ratio

    img.draw_cross(int(x_revise) + 80 , int(y_revise) + 60, color = (255,255,0) , size = 3 )
    print(str(detect_status)+" "+str(x_revise)+" "+str(y_revise)+" "+str(line_ratio))


def part11():##detect green area
    img = sensor.snapshot()

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 90

    COLOR = 0
    X = 0
    Y = 0
    ANGLE = 90

    X_AREA = int(15)
    Y_AREA = int(12)
    img.draw_edges([(80-X_AREA,60+Y_AREA),(80-X_AREA,60-Y_AREA),(80+X_AREA,60-Y_AREA),(80+X_AREA,60+Y_AREA)], color=(255,0,0))


    blue_blobs = img.find_blobs([blue_sandbag],merge=True,margin = 10)
    green_blobs = img.find_blobs([green_sandbag],merge=True,margin = 10)

    img.draw_string(80,60,"P11",color=(0,0,0),size = 1)

    for a in green_blobs:
        if(a.area() < 4600):
            continue
        COLOR = 1
        img.draw_edges(a.min_corners(), color=(0,255,0))
        X = a.cx() - 80
        Y = a.cy() - 60

    for a in blue_blobs:
        if(a.area() < 4600):
            continue
        COLOR = 1
        img.draw_edges(a.min_corners(), color=(0,0,255))
        X = a.cx() - 80
        Y = a.cy() - 60

    img.draw_cross(int(X) + 80 , int(Y) + 60, color = (255,255,0) , size = 3 )
    output_data[0] = COLOR
    output_data[1] = X
    output_data[2] = Y
    output_data[3] = ANGLE

def part12():##detect green area
    img = sensor.snapshot()

    output_data[0] = 0
    output_data[1] = 0
    output_data[2] = 0
    output_data[3] = 90

    COLOR = 0
    X = 0
    Y = 0
    ANGLE = 90

    X_AREA = int(11)
    Y_AREA = int(11)
    img.draw_edges([(80-X_AREA,60+Y_AREA),(80-X_AREA,60-Y_AREA),(80+X_AREA,60-Y_AREA),(80+X_AREA,60+Y_AREA)], color=(255,0,0))

    blue_blobs = img.find_blobs([blue_sandbag],merge=True,margin = 10)
    green_blobs = img.find_blobs([green_sandbag],merge=True,margin = 10)

    img.draw_string(80,60,"P12",color=(0,0,0),size = 1)

    for a in green_blobs:
        if(a.area() < 1000):
            continue
        COLOR = 1
        img.draw_edges(a.min_corners(), color=(0,255,0))
        X = a.cx() - 80
        Y = a.cy() - 60

    for a in blue_blobs:
        if(a.area() < 1000):
            continue
        COLOR = 1
        img.draw_edges(a.min_corners(), color=(0,0,255))
        X = a.cx() - 80
        Y = a.cy() - 60

    img.draw_cross(int(X) + 80 , int(Y) + 60, color = (255,255,0) , size = 3 )
    output_data[0] = COLOR
    output_data[1] = X
    output_data[2] = Y
    output_data[3] = ANGLE

MODE = 0
C_DECODE = 0

while(1):
    part72()

while(1):
    try:
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
        elif(MODE == 72):
            part72()
        elif(MODE == 73):
            part73()
        elif(MODE == 8):
            part8()
        elif(MODE == 9):
            part9()
        elif(MODE == 10):
            part10()
        elif(MODE == 11):
            part11()
        elif(MODE == 12):
            part12()
        else:
            print("No mode select")

        send_data =  str(output_data[0]) + ";" + str(output_data[1]) + ";" + str(output_data[2]) + ";" + str(output_data[3])

    except Exception as err:
        output_data[0] = 0
        output_data[1] = 0
        output_data[2] = 0
        output_data[3] = 90
        send_data =  str(output_data[0]) + ";" + str(output_data[1]) + ";" + str(output_data[2]) + ";" + str(output_data[3])
        print("error(0)")

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
        print("error(1)")
        pass
