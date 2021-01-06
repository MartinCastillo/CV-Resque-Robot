#Está en stand by hasta que se presiona el botón, la luz indica
import RPi.GPIO as GPIO
from time import sleep
GPIO.setwarnings(False) # Ignore warning for now
GPIO.setmode(GPIO.BCM)
len_buffer = 10
button_buffer = []
#pins
button_pin = 17
green_led_pin = 27
red_led_pin = 26
GPIO.setup(button_pin, GPIO.IN,pull_up_down=GPIO.PUD_UP)
GPIO.setup(green_led_pin, GPIO.OUT)
GPIO.setup(red_led_pin, GPIO.OUT)

check_all_equal = lambda x:(len(x) > 0 and all(elem == x[0] for elem in x))

def stand_by()
if(__name__=='__main__'):
    print("Stand by")
    #Color verde
    GPIO.output(red_led_pin,False)
    GPIO.output(green_led_pin,True)
    while(True):
        signal = GPIO.input(button_pin)
        button_buffer.append(signal)
        if(len(button_buffer)> len_buffer):
            del button_buffer[0]
            if(check_all_equal(button_buffer))and(button_buffer[0]==0):
                #Boton pulsado
                break
    print("End Stand by")
    #Color rojo
    GPIO.output(red_led_pin,True)
    GPIO.output(green_led_pin,False)
    sleep(1)
    GPIO.cleanup()
