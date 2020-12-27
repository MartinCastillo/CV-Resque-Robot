"""Un visualizador para el streaming por zmq"""
import cv2
import imagezmq
import numpy as np
image_hub = imagezmq.ImageHub()

while True:
    rpi_name, image = image_hub.recv_image()
    cv2.imshow(rpi_name, image) # 1 window for each RPi
    cv2.waitKey(1)
    image_hub.send_reply(b'OK')
