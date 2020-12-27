#El objetivo de este código es aprovechar una camara de video para seguir una
#linea con los pixeles de la cámara.
"""y si no usas solo una línea?"""
import numpy as np
import cv2
from red_finder import red_finder
last_line_position = 0
black = 0 ; white = 255
parametro_threshold = 90
#Cantidad minima de pixeles rojos en la linea para considerarlo un cuadro rojo
min_red_pixels = 20
delta_red = 50

def find_full_color(list,target_value,full_color_portion=5/4):
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

if(__name__ == '__main__'):
    cap = cv2.VideoCapture(0)
    while True:
        ret,frame = cap.read()
        gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        gray = cv2.GaussianBlur(gray, (5, 5), 0)

        #Saca dos threshold y busca lo que está en común
        #Hace treshold 0 son negros y 255 blancos
        #Y si hace un threshold adaptativo???
        #ret, threshold = cv2.threshold(gray,parametro_threshold ,white,cv2.ADAPTIVE_THRESH_GAUSSIAN_C ,
        #    cv2.THRESH_BINARY,100,2)
        #ret, threshold = cv2.threshold(gray,parametro_threshold ,white,100,2)
        ##ret, threshold_otsu = cv2.threshold(blur,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
        ##threshold = cv2.bitwise_and(threshold,threshold_otsu)

        #Filtro Opening(erosión y dilatasión)
        #Segmenta original y la con threshold
        kernel = np.ones((5,5),np.uint8)
        threshold = cv2.morphologyEx(threshold, cv2.MORPH_OPEN, kernel)

        (imx,imy,size) = frame.shape
        n = 10
        pila = 0
        for i in range(n):
            line = threshold[imy//2-n][:]
            color_line = frame[imy//2-n][:]
            #Encuentra posición de la linea, un valor entre 0 y imx
            #Si no encuentra lineas, retorna el valor de la ultima posición(gap)
            if(black in line):
                line_pos = find_value_mean_position(line,black)
                last_line_position = line_pos
            else:
                line_pos = last_line_position
            pila += line_pos
        line_pos = pila/n
        print(line_pos)

        """Consulta ausenciade linea """
        #Consultar si se llegó a la fase 2
        #Si la mayoria de los cuadros son en negro, 3/4 o así
        if(find_full_color(line,white)):
            pass

        """Consulta Fase 2"""
        #Consultar si se llegó a la fase 2
        #Si la mayoria de los cuadros son en negro, 3/4 o así
        if(find_full_color(line,black)):
            pass

        """Encuentra rojos"""
        """
        #If red line is False, thre is no red square
        red_line = red_finder(color_line,min_red_pixels,)
        if(red_line):
            red_pos = find_value_mean_position(red_line,100)
            #Chequea si la linea está a la derecha o a la izquierda
            print("Rojo: {}".format(red_pos))
            #if(red_pos > line_pos):#Derecha
            else: # A la izquierda
        """
        #Imprime
        cv2.imshow("frame",frame)
        cv2.imshow("threshold",threshold)
        k = cv2.waitKey(1)
        if (ord('k')==k):
            cv2.destroyAllWindows()
            break
    pass
