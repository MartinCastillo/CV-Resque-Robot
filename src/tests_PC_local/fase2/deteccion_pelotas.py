import numpy as np
import cv2
from auto_canny import auto_canny

if(__name__ == '__main__'):
    for i in range(1,16):
        image = cv2.imread("pelotas/suaves/{}.jpg".format(i))
        ###################
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        gray = cv2.pyrUp(cv2.pyrDown(gray))

        edges = auto_canny(gray,sigma = 0.8,kernel = 3)

        im2, contours, hierarchy = cv2.findContours(edges,cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        cv2.drawContours(gray, contours, -1, (0, 255, 0), 3)
        #gray = cv2.GaussianBlur(gray,(5,5),0)
        #edges = cv2.Laplacian(gray,cv2.CV_64F)
        
        cv2.imshow("g1",edges)
        """circles = cv2.HoughCircles(edges,cv2.HOUGH_GRADIENT,1,30,
            param1=45,param2=30,minRadius=0,maxRadius=edges.shape[0])

        if (circles is not None):
            circles = np.round(circles[0,:].astype("int"))
            print("a")
            for i in circles:
                # draw the outer circle
                cv2.circle(image,(i[0],i[1]),i[2],(0,255,0),2)
                # draw the center of the circle
                cv2.circle(image,(i[0],i[1]),2,(0,0,255),3)
        """
        cv2.imshow("g2",gray)
        #cv2.imshow("e",edges)
        k = cv2.waitKey(0)
        if (ord('k')==k):
            cv2.destroyAllWindows()
        if (ord('q')==k):
            cv2.destroyAllWindows()
            break
    pass
