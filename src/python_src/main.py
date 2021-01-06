from picamera.array import PiRGBArray
from picamera import PiCamera
from fase2 import fase2
from time import sleep
import cv2
import numpy as np
import atexit
"""import subprocess
from crash_watchdog.file.file import File
from crash_watchdog.file.crash_watchdog import check_state<<
stat_storage = File()
path_stat_file = r'{}'.format(os.getcwd()+"/"+os.path.basename(__file__))"""
#subprocess.call("raspivid -t 999999 -h 720 -w 1080 -fps 30 -hf -vf -b 2000000 -o - | gst-launch-1.0 -v fdsrc ! h264parse ! queue ! rtph264pay config-interval=1 pt=96 ! gdppay ! tcpserversink host= port=5000")

from Arduino.Arduino import Arduino
arduino = Arduino(baudrate=9600,timeout=1)
ret = arduino.setup(port= 0)

arduino.write("Is90F")
arduino.write("Im0&0F")
#Line following & camera variables
rango_reescalar = 4
last_line_position = 0
black = 0 ; white = 255
parametro_threshold = 90
#Cantidad minima de pixeles rojos en la linea para considerarlo un cuadro rojo
min_red_pixels = 20
delta_red = 50
camera = PiCamera()
camera.rotation = 180
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera,size=camera.resolution)
sleep(0.1)
#variables físicas
high_vel = 250
low_vel = 165
pi = 3.1415926535897932384626433832795
diameter = 17 #cm (actualizalo)
#Distance for a given angle
angle_to_distance = lambda th: (pi*diameter*th)/360
v_l = 0 ; v_r = 0

@atexit.register
def doAtExit():
    #Luego puedes guardar en que parte del código se quedó cuando se serro en un .txt o .csv
    #de manera de retomar al salir
    print("[Interrupt]")
    arduino.write("Is90F")
    arduino.write("Im0&0F")
    rawCapture.truncate(0)
    camera.close()
    arduino.close()
    cv2.destroyAllWindows()
    #subprocess.call([executable,repr(path_stat_file)[1:-1],'htmlfilename.htm'])

def find_full_color(list,target_value,full_color_portion=4/5):
    #Get the portion of the black or white pixels in the line, if its  more than
    #the parameter portion its considered full of that value and returns True
    if target_value in list:
        value_count = 0
        for value in list:
            if value == target_value:
                    value_count+=1
        portion = value_count/len(list)
        if portion > full_color_portion:
            return True
    return False

def find_value_mean_position(list,target_value):
    #Find the index of the list of the center of certain value,
    #e.g: if the line are the value 255, find the center of the line in the list
    n_target_match = 0
    target_sum = 0
    for (ix,value) in enumerate(list):
        if value == target_value:
            target_sum += ix
            n_target_match += 1
    return(target_sum//n_target_match)

for Frame in (camera.capture_continuous(rawCapture,format="bgr",use_video_port=True)):
    frame = Frame.array
    gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)
    ret, threshold = cv2.threshold(gray,parametro_threshold ,white,cv2.THRESH_BINARY)
    ret, threshold_otsu = cv2.threshold(gray,black,white,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
    threshold = cv2.bitwise_and(threshold,threshold_otsu)
    kernel = np.ones((5,5),np.uint8)
    threshold_ = cv2.morphologyEx(threshold, cv2.MORPH_OPEN, kernel)

    (imx,imy,size) = frame.shape
    line = threshold[imy//2][:]

    if(find_full_color(line, 0,4/5)):
        fase2()
    #Encuentra posición de la linea, un valor entre 0 y imx
    #Si no encuentra lineas, retorna el valor de la ultima posición(gap)
    if(black in line):#evitar dividir por 0 en find_value_mean_position()
        line_pos = find_value_mean_position(line,black)
        last_line_position = line_pos
    else:
        line_pos = last_line_position
    medio = imx//2
    delta = abs(line_pos-medio)
    if line_pos > medio:
        print('Girar derecha/Pos linea: {}/Delta a izquierda: {}'.format(line_pos,delta))
        v_r = -170; v_l = 170
    else:
        print('Girar izquierda/Pos linea: {}/Delta a derecha: {}'.format(line_pos,delta))
        v_r = 170; v_l = -170
    arduino.move_motors(v_l,v_l)
    sleep(0.01)

    rawCapture.truncate(0)
camera.close()
arduino.close()
