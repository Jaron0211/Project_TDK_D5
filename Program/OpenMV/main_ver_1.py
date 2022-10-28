import  sensor, image, time , math

thresholds = (220, 250)
min_degree = 0
max_degree = 180

DETECT_R = 5

sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_whitebal(False)

clock = time.clock()

def  two_circle_detection():
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

            B =A*l.line()[0]  -  l.line()[1]

            apple203 = abs(A*C.x() -C.y() -B) / math.sqrt (A**2 + 1)

            if ( apple203 < DETECT_R):
                    img.draw_line((C.x(),C.y(),l.line()[0],l.line()[1]),color = (255,0,0))
                    print((C.x(),C.y(),l.line()[0],l.line()[1]))

            else:
                    img.draw_line(l.line(),color = (100,76,0))
                    
        img.draw_circle(C.x(),C.y(),C.r(),color = (0,255,0))

while(1):
    
    
