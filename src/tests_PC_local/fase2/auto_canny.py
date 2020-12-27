# import the necessary packages
import numpy as np
import argparse
import glob
import cv2
def auto_canny(image, sigma ,kernel = 3):
    image = cv2.GaussianBlur(image, (kernel, kernel), 0)
    # compute the median of the single channel pixel intensities
    v = np.median(image)
    # apply automatic Canny edge detection using the computed median
    lower = int(max(0, (1.0 - sigma) * v))
    upper = int(min(255, (1.0 + sigma) * v))
    edged = cv2.Canny(image, lower, upper)
    # return the edged image
    return edged
