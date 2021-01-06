import numpy as np
import cv2
from auto_canny import auto_canny
def recoger():
    #Movimiento servo 3 con feedback de sensor de distancia

def fase2():
    from Arduino.Arduino import Arduino
    arduino = Arduino(baudrate=9600,timeout=1)
    ret = arduino.setup(port= 0)
    arduino.write("Is90F")
    arduino.write("Im0&0F")

    cap = cv2.VideoCapture(0)
    #Factor para pixelar la imagen manteninendo la raelación de aspecto
    factor_reescalado = 4
    #Respecto al denoise multiple almacena frames anteriores
    pila_denoise = []
    len_pila_denoise = 3
    while True:
        margin = 30
        in_front = 100
        ret,frame = cap.read()
        resized = cv2.resize(frame,(frame.shape[1]//factor_reescalado,frame.shape[0]//factor_reescalado))
        resized = cv2.resize(resized,(resized.shape[1]*factor_reescalado,resized.shape[0]*factor_reescalado))
        #Denoise multiple
        dst = resized
        pila_denoise.append(resized)
        if(len(pila_denoise)>len_pila_denoise):
            #https://docs.opencv.org/3.4/d1/d79/group__photo__denoise.html#ga723ffde1969430fede9241402e198151
            dst = cv2.fastNlMeansDenoisingMulti(pila_denoise, len(pila_denoise)-1, (len_pila_denoise-1)//2, None, 4, 7, 5)
            del pila_denoise[0]

        blur = cv2.GaussianBlur(dst,(5,5),0)
        gray = cv2.cvtColor(blur, cv2.COLOR_BGR2GRAY)
        edges = auto_canny(gray,sigma = 0.7)
        circles = cv2.HoughCircles(edges,cv2.HOUGH_GRADIENT,1,35,
            param1=50,param2=30,minRadius=0,maxRadius=edges.shape[1])

        if (circles is not None):
            circles = np.round(circles[0,:].astype("int"))
            for i in circles:
                #cv2.circle(gray,(i[0],i[1]),i[2],(0,255,0),2)
                #cv2.circle(gray,(i[0],i[1]),2,(0,0,255),3)
                while(check_size()<in_front):
                    if(i[1]>frame.shape[1]/2+margin):
                        arduino.move_motors(240,0)
                    elif(i[1]<frame.shape[1]/2-margin):
                        arduino.move_motors(0,240)
                    else:
                        arduino.move_motors(240,240)
                    sleep(0.01)
                recoger()

        res = np.hstack((gray,edges))
        #cv2.imshow("resized",res)
        cv2.imshow("dst",res)
        k = cv2.waitKey(1)
        if (ord('k')==k):
            cv2.destroyAllWindows()
            break
    pass
