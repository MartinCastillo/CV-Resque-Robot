"""This file contains Arduino´s serial class that contains all the functions and
parameters needed to establish a serial comunication with an arduino"""
import serial
import atexit
from time import sleep

class Arduino:
    def __init__(self,baudrate=9600,timeout=1,robot_diameter=18.5):
        self.baudrate = baudrate
        self.timeout = timeout
        self.robot_diameter = robot_diameter
        self.arduino = None

    def doAtExit(self):
        """En el caso de que el programa se cierre de una forma no programada"""
        if self.arduino.isOpen():
            #Actuadores a la normalidad
            self.move_motors(0,0)
            self.move_servo(90)
            self.arduino.close()
            print("The serial has been closed  and the actuators restored succesfully")
            print("self.arduino.isOpen() = " + str(self.arduino.isOpen()))

    def setup(self,port=1):
        atexit.register(self.doAtExit)
        try:
            self.arduino = serial.Serial(
                port='/dev/ttyUSB{}'.format(port),
                baudrate = self.baudrate,
                timeout = self.timeout
                )
            while (self.arduino.isOpen()==False):
                print( "Esperando puerto para abrir...\n")
            print('Puerto serial conectado a {}'.format(self.arduino.name))
            return True
        except:
            print("Problemas abriendo el puerto")
            return False

    def request_arduino(self,request,timeout=1):
        self.arduino.write(bytes(request, 'utf-8'))
        if (self.arduino.inWaiting()>0):
            return (self.arduino.readline())
        #Hacer un timeout
        return False

    def write(self,message):
        """
        Formato de mensajes,
        ___________________
        -Al inicio de todos los mensajes hay una 'I'
        -Los mensajes terminan en 'F'
        -Todos los mensajes de motor suigen con "m" y solo son para mover indefinidamente las ruedas
        con velocidades dadas de -255 a 255 cada una independientemete, sepearadas por "&", el encoder
        se conecta a la raspberry y ahí se ve el tema de distancias; Im255&-255F , Im255&0F
        -Si es una "R"; es un request de sensores y se devuelve con los sensores separados por "&";
        25.9&33&90.2
        -Si es una "s", es un movimiento de servo, y le sigue el angulo deseado; Is90F
        """
        self.arduino.write(bytes(message,'utf-8'))

    def move_motors(self,v_L,v_R):
        #Envia la señal a arduino de mover los motores de manera indefinida
        """Más adelante podrias enviar un request de que un aproximado de la
         velocidad si se cumple"""
        self.write("Im"+str(v_L)+"&"+str(v_R)+"F")

    def turn_angle(self,angle,vel,dir):
        #dir = 1 derecha, dir = -1 izquierda
        pi = 3.1415926535897932384626433832795
        l = (pi*self.robot_diameter*angle)/360
        self.write("Id{}v{}D{}F".format(l,vel,dir))

    def move_motors_distance(self,distance,vel,dir):
        #dir = 0 adelante, dir = 2 atras
        self.write("Id{}v{}D{}F".format(distance,vel,dir))

    def move_servo(self,ang):
        self.write("Is"+str(ang)+"F")

    def close(self):
        self.arduino.close()

if (__name__ == "__main__"):
    pass
