# Find Circles Example
#
# This example shows off how to find circles in the image using the Hough
# Transform. https://en.wikipedia.org/wiki/Circle_Hough_Transform
#
# Note that the find_circles() method will only find circles which are completely
# inside of the image. Circles which go outside of the image/roi are ignored...

import sensor, image, time

thresholds = (220, 250)
min_degree = 0
max_degree = 179

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot().binary([thresholds], invert=0, zero=True)#.lens_corr(1.8)
    print(img)
    img.gaussian(0)
    # Circle objects have four values: x, y, r (radius), and magnitude. The
    # magnitude is the strength of the detection of the circle. Higher is
    # better...

    # `threshold` controls how many circles are found. Increase its value
    # to decrease the number of circles detected...

    # `x_margin`, `y_margin`, and `r_margin` control the merging of similar
    # circles in the x, y, and r (radius) directions.

    # r_min, r_max, and r_step control what radiuses of circles are tested.
    # Shrinking the number of tested circle radiuses yields a big performance boost.

    for c in img.find_circles(threshold = 2000, x_margin = 40, y_margin = 40, r_margin = 20,
            r_min = 10, r_max = 35, r_step = 5):

          img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
          print(c)
    for l in img.find_lines(threshold = 1500, theta_margin = 30, rho_margin = 60):
            if (min_degree <= l.theta()) and (l.theta() <= max_degree):
                img.draw_line(l.line(), color = (255, 0, 0))
                print(l.theta())

    print("FPS %f" % clock.fps())
