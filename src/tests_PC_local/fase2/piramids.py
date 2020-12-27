import numpy as np
import cv2
from auto_canny import auto_canny

if(__name__ == '__main__'):
    for i in range(1,16):
        image = cv2.imread("pelotas/suaves/{}.jpg".format(i))
        ###################
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        res = gray
        res = cv2.pyrUp(cv2.pyrDown(res))

        edges = auto_canny(res,sigma = 0.8,kernel = 5)
        cv2.imshow("g1",edges)
        k = cv2.waitKey(0)
        if (ord('k')==k):
            cv2.destroyAllWindows()
        if (ord('q')==k):
            cv2.destroyAllWindows()
            break
    pass
