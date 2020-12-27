#https://pimylifeup.com/raspberry-pi-serial/
import serial
import atexit
from time import sleep
#for n in range(4):
n = 0
try:
    arduino = serial.Serial(
        port='/dev/ttyUSB{}'.format(n),
        baudrate = 9600,
        timeout = 3.0
        )
except:
    pass
while (arduino.isOpen()==False):
    print( "Esperando puerto para abrir...\n")
print('Puerto serial conectado a {}'.format(arduino.name))

def doAtExit():
    """en el caso de que el programa se cierre de una forma no programada"""
    global arduino
    if arduino.isOpen():
        #Actuadores a la normalidad
        arduino.write(bytes("Is90F",'utf-8'))
        arduino.write(bytes("Im0&0F",'utf-8'))
        arduino.close()
        print("The serial has been closed  and the actuators restored succesfully")
        print("arduino.isOpen() = " + str(arduino.isOpen()))
atexit.register(doAtExit)

def request_arduino(request,timeout=1):
    arduino.write(bytes(request, 'utf-8'))
    if (arduino.inWaiting()>0):
        return arduino.readline()
    #Hacer un timeout
    return False
"""
Formato de señales1,
-Al inicio de todos los mensajes hay una 'I'
-Los mensajes terminan en 'F'
-Todos los mensajes de motor suigen con "m" y solo son para mover indefinidamente las ruedas
con velocidades dadas de -255 a 255 cada una independientemete, sepearadas por "&", el encoder
se conecta a la raspberry y ahí se ve el tema de distancias; Im255&-255F , Im255&0F
-Si es una "R"; es un request de sensores y se devuelve con los sensores separados por "&";
25.9&33&90.2
-Si es una "s", es un movimiento de servo, y le sigue el angulo deseado; Is90F
"""
def mover_motores(v_L,v_R):
    #Envia la señal a arduino de mover los motores de manera indefinida
    """Más adelante podrias enviar un request de que un aproximado de la
     velocidad si se cumple"""
    arduino.write(("Im"+str(v_L)+"&"+str(v_R)+"F").encode('utf-8'))

if(__name__=="__main__"):
    while True:
        arduino.write(bytes("Im130&130F",'utf-8'))
        sleep(1)
        arduino.write(bytes("Im0&0F",'utf-8'))
        sleep(2)
    pass
