"""
When this program is closed in an unprogrammed manner, it is opened again, butfirst it
saves a variable in the stat.txt file that belongs to the same directory, this variable
indicates in which part of the code was left the execution before it was closed. If e.g.
the raspberry pi shuts down unexpectedly, the program starts from where it left off (You
should set the code to run with the raspberry boot)."""
import subprocess
import atexit
import os
from sys import executable
from file.file import File

from time import sleep

path_stat_file = r'{}'.format(os.getcwd()+"\\"+os.path.basename(__file__))
#Actualiza la variable de estado del código, cuando se cambia stat_storage.stat,
#El archivo cambia
stat_storage = File()
print('Iniciando programa en sección {}'.format(stat_storage.stat))

@atexit.register
def callback():
    print("Salida del código sin señal, reiniciando programa en sección {}".format(stat_storage.stat))
    #https://stackoverflow.com/questions/25651990/oserror-winerror-193-1-is-not-a-valid-win32-application
    subprocess.call([executable,repr(path_stat_file)[1:-1],'htmlfilename.htm'])

#Decorator for the fase's functions, it checks if the state in the file is equal to the fase´s state,
#if thats the case, the fase´s function is executed, when the function finishes, the state.txt file
#is updated by the decorator, if there is an interruption the next function is the one with
#the same state that is in the stat.txt, aka: each time that the program is ran, it starts from the
#las fase.
def check_state(*args,**kargs):
    def decorator(func):
        def wrapper():
            if(stat_storage.stat==kargs['target_stat']):
                func()
                stat_storage.stat = kargs['next_stat']
        return wrapper
    return decorator

#E.g:
@check_state(target_stat=1,next_stat=2)
def fase1():
    for i in range(10):
        sleep(0.03)
        print("Fase1")

@check_state(target_stat=2,next_stat=3)
def fase2():
    for i in range(10):
        sleep(0.03)
        print("Fase2")

#Last next_stat have to be also the first target_stat if you want the code to
#start from the beginning when you restart it.
@check_state(target_stat=3,next_stat=1)
def fase3():
    for i in range(10):
        sleep(0.03)
        print("Fase3")

if(__name__ == '__main__'):
    fase1()
    fase2()
    fase3()
    os._exit(0)
