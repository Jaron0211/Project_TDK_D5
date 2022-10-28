# Find Lines Example
#
# This example shows off how to find lines in the image. For each line object
# found in the image a line object is returned which includes the line's rotation.

# Note: Line detection is done by using the Hough Transform:
# http://en.wikipedia.org/wiki/Hough_transform
# Please read about it above for more information on what `theta` and `rho` are.

# find_lines() finds infinite length lines. Use find_line_segments() to find non-infinite lines.

enable_lens_corr = False # turn on for straighter lines...

import sensor, image, time
import time
from pyb import UART

uart = UART(3, 19200)

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)

sensor.set_contrast(+2)
sensor.set_brightness(+3)
sensor.set_auto_gain(True)

sensor.skip_frames(time = 2000)
clock = time.clock()

min_degree = 0
max_degree = 179



while(True):
    clock.tick()

    img = sensor.snapshot()

    line_img = sensor.get_fb()
    cir_img = sensor.get_fb()

    cir_img.gaussian(2)
    cir_img.binary([(40,100)])

    if enable_lens_corr: img.lens_corr(1.8) # for 2.8mm lens...

    for c in cir_img.find_circles(threshold = 4000, x_margin = 10, y_margin = 10, r_margin = 60,
                    r_min = 2, r_max = 20, r_step = 4):
                img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                print(c)


    for l in line_img.find_lines(threshold = 1200, theta_margin = 30, rho_margin = 50):
        if (min_degree <= l.theta()) and (l.theta() <= max_degree):
            img.draw_line(l.line(), color = (255, 0, 0))
            x = l.line()[0]
            print(l.theta())
        print("FPS %f" % clock.fps())

