import serial
import time
import atexit

def request_arduino(request,timeout=1):
    #for i in range(3):#Trata el reques cierta cantidad de veces
    arduino.write(bytes(request, 'utf-8'))
    #Hacer un timeout
    _timeout = time.time() + timeout #sec(s) from now
    buff = ""
    while(True):
        read = arduino.read().decode("utf-8") 
        if(read):
            if(read=='\n'):
                return buff
            else:
                buff += read
        if(time.time() > _timeout):
            break
    return False

def doAtExit():
    global arduino
    if arduino.isOpen():
        #Vuelve actuadores a la normalidad
        arduino.write(bytes("Is90F",'utf-8'))
        arduino.write(bytes("Im0&0F",'utf-8'))
        arduino.close()
        print("Close serial")
        print("arduino.isOpen() = " + str(arduino.isOpen()))
atexit.register(doAtExit)

arduino = serial.Serial(
    port = 'COM4',
    baudrate = 9600,
    timeout = 3.0
        )

while (arduino.isOpen()==False):
    print( "Esperando puerto para abrir...\n")
print('Puerto serial conectado a {}'.format(arduino.name))


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

def write(msg):
    arduino.write(bytearray(msg,'utf-8'))
    arduino.flush()
    r = arduino.read()
    return r

if(__name__=="__main__"):
    while True:
        print(request_arduino("r"))
        """
        arduino.write(bytes("Is45F",'utf-8'))
        time.sleep(3)
        arduino.write(bytes("Is90F",'utf-8'))
        time.sleep(3)
        """
    pass
