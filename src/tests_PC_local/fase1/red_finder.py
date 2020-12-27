#Tiene funciones para encontrar el color rojo en una imagen, si lo hay
#tiene funciones para ver en que parte de cierta linea
import numpy as np
import cv2
def red_finder(color_line,red_pixels,delta_red):
    #El formato es BGR, red_pixels son los pxeles minimos para
    #considerar una linea
    #0 son no rojo, 255 son rojos
    red_pixels_count = 0
    red_line = np.zeros(len(color_line))
    for (ix,pixel) in enumerate(color_line):
        delta_green = pixel[2] - pixel[1]
        delta_blue = pixel[2] - pixel[0]
        if(delta_blue >  self.delta_red)and(delta_green > self.delta_red):
            red_line[ix] = 255
            red_pixels_count += 1
    if red_pixels_count >= red_pixels:
        return red_line
    return False
