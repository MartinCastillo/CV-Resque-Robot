import RPi.GPIO as GPIO
from Arduino.Arduino import Arduino
from time import sleep
arduino = Arduino()
arduino.setup()
high_vel = 250
low_vel = 165

pulses_encoder = 8
reduction = 48
rpm_max = 120
pin_encoder_l=26
pin_encoder_r=19
#bouncetime_calc = int((1000*(rpm_max/60))//(reduction*pulses_encoder))
last_state_l = 0
last_state_r = 0
encoder_counter_l = 0
encoder_counter_r = 0
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_encoder_l, GPIO.IN,pull_up_down=GPIO.PUD_UP)
GPIO.setup(pin_encoder_r, GPIO.IN,pull_up_down=GPIO.PUD_UP)
def callback_l(chanel):
    global last_state_l,encoder_counter_l,pin_encoder_l
    #Chequea si el estado ha cambiado y si este es de "subida" solamente
    stat = GPIO.input(chanel)
    if(stat!=last_state_l):
        last_state_l = stat
        encoder_counter_l+=1

def callback_r(chanel):
    global last_state_l,encoder_counter_l,pin_encoder_l
    #Chequea si el estado ha cambiado y si este es de "subida" solamente
    stat = GPIO.input(chanel)
    if(stat!=last_state_l)and(stat!=0):
        last_state_l = stat
        encoder_counter_l+=1
        #print("r:{}".format(encoder_counter_r))

GPIO.add_event_detect(pin_encoder_l, GPIO.RISING, callback=callback_l)
GPIO.add_event_detect(pin_encoder_r, GPIO.RISING, callback=callback_r)
#Muestreo encoder calculado según la máxima cantidad de pulsos por segundo, y ese tiempo a  millisegundos
distance = 5#cm
pulses = 100

print("Start")
while True:
    if(encoder_counter_r >= pulses):
        print("End")
        break;
    pass
