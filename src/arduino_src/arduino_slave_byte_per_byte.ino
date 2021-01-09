#include <Adafruit_VL53L0X.h>
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#include <Servo.h>
Servo servo_camera;
//Este código es el que se corre en arduino, se encarga de seguir las ordenes de
//la raspberry, ya sea avanzar una distancia concreta(encoders), avanzar(diferentes
//velocidades por rueda), parar, y con la señal adecuada, envia un mensaje de todos
//los sensores en la placa, como lista, que luego raspberry procesa
///////////////////////////////////////////////////////////////////////////////////
//Pines motor B
const byte BIA = 6; //Gris
const byte BIB = 5; //Amarillo
//Pines motor A
const byte AIA = 11;//Morado
const byte AIB = 3; //Azul

const byte pin_servo_camera = 8;
boolean busqueda = true;
String buff = "";
//Encoders
//Izquierda
#define ENCODER_I 2
#define RADIO_RUEDA 6.6 //cm
#define pi 3.1415926535897932384626433832795
#define pulses_revolution (48*8)
volatile unsigned long encoder_i_count = 0;

//volatile boolean state_i = false;
void encoder_I() {
  //if (state_i) {
  encoder_i_count += 1;
  //}
  //state_i = !(state_i);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  pinMode(ENCODER_I, INPUT);
  pinMode(BIA, OUTPUT);
  pinMode(BIB, OUTPUT);
  pinMode(AIA, OUTPUT);
  pinMode(AIB, OUTPUT);
  pinMode(13, OUTPUT);
  servo_camera.attach(pin_servo_camera);
  if (!lox.begin()) {
    //No se pudo iniciar el VL53L0X
  }
  attachInterrupt(digitalPinToInterrupt(ENCODER_I), encoder_I, RISING);
}

void loop() {
  //Inicio de mansajes 'I', final 'F'
  busqueda = true;
  buff = "";
  if (Serial.available()) {
    if (Serial.read() == 'I') {
      //73 es I en Ascci, abre la busqueda de intermediarios
      while (true) {
        if (Serial.available()) {
          //Espera a los demás datos de la trama
          char temp_buff = (char)Serial.read();
          if (temp_buff == 'F') {
            //Carácter de final
            busqueda = false;
            break;
          }
          else if ((temp_buff != '\n') && (temp_buff != ' ')) {
            buff += temp_buff;
          }
        }
      }
    }
  }

  if (busqueda == false) {
    //Aquí en teoría ya se tiene el mensaje en limpio sin F o I, listo para ejecutarse
    if (buff.startsWith("m")) {
      //Movimiento motores
      int indice_separador = buff.indexOf("&");
      int v_l = buff.substring(1, indice_separador).toInt();
      int v_r = buff.substring(indice_separador + 1).toInt();
      move_motors(v_l, v_r);
      //Serial.println(v_l);
      //Serial.println(v_r);
    }
    if (buff.startsWith("d")) {
      //Mensaje para mover las ruedas una distancia, velocidad y dirección
      //Ej : Id20v240D1F
      //Movimiento motores
      int indice_vel = buff.indexOf("v");
      int indice_dir = buff.indexOf("D");
      int distance = buff.substring(1, indice_vel).toFloat();
      int vel = buff.substring(indice_vel + 1, indice_dir).toInt();
      //Si dir vale -1 es hacia la izquierda, 0 es hacia adelante, 1 a la derecha, 2 hacia atrás
      int dir = buff.substring(indice_dir + 1).toInt();
      int dir_i = 1;
      int dir_d = 1;

      switch (dir) {
        case -1:
          dir_i = -1; dir_d = 1;
          break;
        case 0:
          dir_i = 1  ; dir_d = 1;
          break;
        case 1:
          dir_i = 1; dir_d = -1;
          break;
        case 2:
          dir_i = -1; dir_d = -1;
          break;
      }

      move_distance(distance, vel, dir_i, dir_d);
      //Serial.println(v_l);
      //Serial.println(v_r);
    }
    else if (buff.startsWith("s")) {
      int angle = buff.substring(1).toInt();
      mover_servo_pausado(servo_camera, angle, 10);
      //Serial.println(angle);
    }
    else if (buff.startsWith("r")) {
      //Request de sensores
      Serial.println("Response");
    }
  }
}

//Funciones motores
//Función para mover el servo por pasos
void mover_servo_pausado(Servo servo, int angle, int delay_paso) {
  if (servo.read() != angle) {
    //Suma o resta al angulo actual
    int steps_direction = (angle > servo.read()) ? 1 : -1;
    for (int a = servo.read(); a != angle; a += steps_direction) {
      servo.write(a);
      delay(delay_paso);
    }
  }
}

void move_distance(float distance, int vel, int dir_i, int dir_d) {
  //dir_i y dir_d es la dirección de cada rueda, si es 1 es adelante, -1 atras, solo usa el encoder izquierdo
  encoder_i_count = 0;
  int pulses = (pulses_revolution * distance) / (pi * RADIO_RUEDA);
  while (encoder_i_count < pulses) {
    move_motors(vel * dir_i, vel * dir_d);
  }
  move_motors(-vel * dir_i, -vel * dir_i);
  delay(50);
  move_motors(0, 0);
}

void stop_motors() {
  digitalWrite(AIA, LOW); analogWrite(AIB, LOW);
  digitalWrite(BIA, LOW); analogWrite(BIB, LOW);
}

void move_motors(int speed_L, int speed_R) {
  if ((speed_L == 0) && (speed_R == 0)) {
    stop_motors();
  }
  else {
    //Esta función según el signo de la velocidad hace avanzar o girar ambos motores independientemente
    bool dir_R  = (speed_R >= 0) ? 1 : 0;
    bool dir_L  = (speed_L >= 0) ? 1 : 0;
    if (dir_L)forward(1, speed_L);
    else backward(1, abs(speed_L));
    if (dir_R)forward(2, speed_R);
    else backward(2, abs(speed_R));
  }
}

void forward(int motor, int _speed) {
  //Esta función hace avanzar un motor específico
  if (motor == 2) {//Derecho
    analogWrite(AIA, _speed); digitalWrite(AIB, LOW);
  }
  if (motor == 1) {//Izquierdo
    analogWrite(BIA, _speed); digitalWrite(BIB, LOW);
  }
}

void backward(int motor, int _speed) {
  //Esta función hace retroceder un motor específico
  if (motor == 2) {//Derecho
    digitalWrite(AIA, LOW); analogWrite(AIB, _speed);
  }
  if (motor == 1) {//Izquierdo
    digitalWrite(BIA, LOW); analogWrite(BIB, _speed);
  }
}
