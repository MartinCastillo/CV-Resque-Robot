//http://www.bajdi.com/l9110-h-bridge-module/
/////////////////Arrays sensores/////////////////
//El 0 es a la derecha y 3 izquierda
const int n_sensores = 5;
//El indice 0 es el del sensor de la izquierda
int sensores[n_sensores] = {0, 1, 2, 3, 4};
//Una constante obtenida del promedio de los valores de los sensores frente a una misma condición

//En nuevos entornos reasignar estas variables
int constantes_calibrar[n_sensores] = { 0, 0, 0, 0, 0};
int ret_sensores[n_sensores] = { 0, 0, 0, 0, 0};
int linea[n_sensores] = { 0, 0, 0, 0, 0};
/////////////////Pines motores/////////////////
const int AIA = 5;  // (pwm) pin A-IA Velocidad
const int AIB = 6;  // (pwm) pin A-IB Direción
const int BIA = 11; // (pwm) pin B-IA  Velocidad
const int BIB = 10;  // (pwm)pin B-IB Dirección
/////////////////Umabrales motores/////////////////
int v_blancos[n_sensores] = {50, 53, 51, 54, 60};
int v_rojos[n_sensores] = {200, 200, 200, 200, 200};
int v_negros[n_sensores] = {585, 550, 497, 557, 654};
int umbral = 7;
/////////////////Velocidades motores/////////////////
int speed_Iz = 0;
int speed_Ri = 0;
/////////////////Void setup/////////////////
//valida diferentes partes del codigo según la necesidad, calibrar,
int max_vel = 160;
//principal, testear, etc
bool modo = 1;
//Para girar con señales rojas y para los GAP
bool girar_con_prox_negro = 0;
int  last_action = 0;
int direccion_giro_rojo;

//Constantes encoders, mediciones y variables volatiles
const int pin_encoder_i = 2;//encoder izquierdo
const int pin_encoder_d = 3;//encoder derecho
const int ratio = 48;
const float diameter = 6.5;//cm
const float robot_diameter = 6;//ajustar
const float pi = 3.1415;
const int pulses_per_axis = 4;
const float distancia_90_grados = (pi*diameter * 90) / 360;
volatile unsigned long encoder_count_R = 0;
volatile unsigned long encoder_count_L = 0;
const int pin_alimentacion_puente_h = 7;
//-----------------------------------------------------------
void setup() {
  //Setea los pines de los motores
  pinMode(AIA, OUTPUT);
  pinMode(AIB, OUTPUT);
  pinMode(BIA, OUTPUT);
  pinMode(BIB, OUTPUT);
  for (int i = 0; i < n_sensores; i++) {
    pinMode(i, OUTPUT);
  }
  Serial.begin(9800);
  attachInterrupt(digitalPinToInterrupt(pin_encoder_d), count_R, RISING);
  attachInterrupt(digitalPinToInterrupt(pin_encoder_i), count_L, RISING);
}

//Contadores de estado encoders
void count_R() {
  encoder_count_R++;
}
void count_L() {
  encoder_count_L++;
}
//-----------------------------------------------------------
/////////////////Void loop/////////////////
void loop() {
  ///Recuerda que los arrays son listas de referencia por lo que no es necesario
  ///que estas funciones den retorno de forma explícita
  /*
     ret_sensores : Los valores crudos de los sensores
     linea : Los valores de los sensores pasados por la función evaluar senosres, en dígitos {0,1,2}
     pos: Float con la posición en indices de la línea, si vale -1 es por que no hay línea detectada
  */
  //Actualiza array de los sensores, el segúndo parametro es para si añade o no la calibración
  bool ret = leer_sensores(ret_sensores, 0);
  //linea es un array de cuatro elementos, que contiene si hay o no
  //línea o rojo en un sensor
  ret = evaluar_sensores(linea, ret_sensores);
  //Es un float con el índice teórico en que está el centro de la línea
  //Busca la posición de los valores 1
  float pos = get_line_pos(linea, 1);
  float medio = n_sensores / 2;

  //-----------------------------------------------------------
  //MODO CALIBRAR
  /*if (false) {
    stop_motors();
    //Si está en modo calibrar
    //Obtiene promedio de valores de sensores
    int promedio = 0;
    for (int i = 0; i < n_sensores; i++) {
      promedio += ret_sensores[i];
    }
    promedio /= n_sensores;
    //Asigna la varianza de cada sensor
    for (int i = 0; i < n_sensores; i++) {
      constantes_calibrar[i] = promedio - ret_sensores[i];
    }
    //imprime_lista(constantes_calibrar, String(promedio));
    imprime_lista(ret_sensores, String(pos));
    delay(200);
    }*/
  //-----------------------------------------------------------
  if (true) {
    speed_Iz = max_vel; speed_Ri = max_vel;
    //Hay 5 posiciones, centro, medio derecha-izquierda y derecha-izquierda
    //-----------------------------------------------------------
    //SIGUE LINEA, segun posición de la línea
    if (pos ==  medio) {
      //linea al centro
      speed_Iz = max_vel; speed_Ri = max_vel;
      last_action = 1;
    }
    else if (pos > medio) {
      //linea a la derecha
      speed_Iz = 255; speed_Ri = 0;
      last_action = 0;
    }
    else if ((pos < medio) and (pos != -1)) {
      //linea a la izquierda
      speed_Iz = 0; speed_Ri = 255;
      last_action = 0;
    }
    //-----------------------------------------------------------
    //GAP (Si los 3 en blanco)
    else if ((pos == -1) and (last_action == 1)) {
      //Si pierde la línea luego de estar centrado, sigue recto, si no, sigue  con la acción anterior
      speed_Iz = max_vel; speed_Ri = max_vel;
      Serial.println("Gap");
      last_action = 0;
    }
    //-----------------------------------------------------------
    imprime_lista(linea, String(pos));
    //Serial.println(String(speed_Iz) + " | " + String(speed_Ri));
    move_motors(speed_Iz, speed_Ri);
    delay(1);
    //-----------------------------------------------------------
    //CUADROS ROJOS, (al final, luego de que el robot se mueva)
    if (false) {
      if (arrayIncludeElement(linea, -1)) {
        //Si encuantra un rojo, busca la posición del cuadro rojo(los -1 son rojos)
        float pos_rojo = get_line_pos(linea, -1);
        if (pos > pos_rojo) {
          //El cuadro está a la izquierda de la línea
          direccion_giro_rojo = 0;
        }
        else if (pos < pos_rojo) {
          //El cuadro está a la derecha de la línea
          direccion_giro_rojo = 1;
        }
        girar_con_prox_negro = 1;
      }
      //Si ya no hay rojo y girar_con_prox_negro vale 1
      else if (girar_con_prox_negro == 1) {
        girar_con_prox_negro == 0;
        //Si direccion_giro_rojo es 0 a izquierda, si no a la derecha
        funcion_giro_rojo(direccion_giro_rojo);
      }
    }
    //-----------------------------------------------------------
    //OBSTACULOS
  }
}
//-----------------------------------------------------------
//-----------------------------------------------------------
//FUNCIONES
//-----------------------------------------------------------
void funcion_giro_rojo(int n_sensor) { //El senor derecho o izquierdo para detectar
  //move_motor_distance(distancia_90_grados, -255, 255);
  //Gira hasta que el sensor detecta blanco(0) y luego negro(1), por eso el for, luego avanza un poco
  int vel_d = (n_sensor == 0) ? 255 : -255;
  move_motors(-vel_d, vel_d);
  for (int i = 0; i <= 1; i++) {
    while (linea[n_sensor * (n_sensores - 1)] == i) { //Si la dirección es 0 es el izquierdo, si es 1 es el derecho
      bool ret = leer_sensores(ret_sensores, 0);
      ret = evaluar_sensores(linea, ret_sensores);
      delay(10);
    }
  }
  move_motors(vel_d, -vel_d);//Se detiene bruscamente (Se invierten las velocidades por un tiempo corto)
  delay(50);
  move_motors(0, 0);
  delay(25);
  move_motors(255, 255);
  delay(100);
}
void imprime_lista(int list[n_sensores], String add) {
  //Esta función imprime la lista de parametro de forma ordenada
  // tiene un parametro adicional que es el add
  String B = "|";
  //Imprime los valores de una lista de forma ordenada
  for (int i = 0; i < n_sensores; i++) {
    B += (String(list[i]) + " | ");
  }
  Serial.println(B + " || " + add);
}
//-----------------------------------------------------------
///////////Funciones lógica/////////////////
/*//Esta función está más o menos rota, pero quizá tiene futuro
  bool set_velocity_by_line(float _pos) {
  //Toma la ´posición´ de la línea y según la fórmula
    //vd = max - ((max/n)*pos) + delta
    //vi = (max/n)*pos + delta
    //obtiene las velocidades vd y vi respectivas, con n como el máximo
    //indice de la lísta de valores, en este caso 3, max es la máxima
    //velocidad de los motores, consideremos que las fórmulas se obtienen
    //desde la base de que cuando la línea se encuentra en los ejes de los
    //bordes, gira con velocidades de 0 y 255 en cada rueda.
    //Luego asigna las velocidades a las variables globales.
  speed_Ri = max_vel - (max_vel / (n_sensores - 1) * _pos); //vd
  speed_Iz = (max_vel / (n_sensores - 1) * _pos); //vi
  //Tammbién nos aseguramos de que resulte en la máxima velocidad posible
  //Ve la distancia de la mayor velocidad con el máximo (255) y se la suma a
  //ambas velocidades.
  int delta = (speed_Ri >= speed_Iz) ? (max_vel - speed_Ri) : max_vel - speed_Iz;
  speed_Ri += delta; speed_Iz += delta;
  return true;
  }*/

float get_line_pos(int _linea[n_sensores], int buscar) {
  //Promedio de valores de puntos negro, para segír la línea
  //Busca los 1 (ahora los 'buscar')
  //ACTUALIZACIÖN: Promedia los valores con el valor buscar

  float ret = 0; float n_negros = 0; //Número de sensores en línea
  if (arrayIncludeElement(_linea, buscar)) { //Si hay negros en la línea
    for (int i = 0; i < n_sensores; i++) {
      if (_linea[i] == buscar) {
        n_negros += 1;
        ret += i;
      }
    }
    return (ret / n_negros);
  }
  else {
    return -1;
  }
}

//Función que robé de internet para ver si hay cierto elemento en un array
boolean arrayIncludeElement(int _array[n_sensores], int element) {
  for (int i = 0; i < n_sensores; i++) {
    if (_array[i] == element) {
      return true;
    }
  }
  return false;
}

//Evalua si es línea, rojo o blanco
bool evaluar_sensores(int _linea[n_sensores], int _ret_sensores[n_sensores]) {
  /*Toma los valores de salida de los sensores y dice si hay línea, rojo o blanco
    Esto según las variables de valor de colores, por lo que también toma los valores
    del array linea, que se pasa por referencia*/
  //Como valores posibles estan: 1 para negro, 0 para blanco y -1 para rojo
  for (int i = 0; i < n_sensores; i++) {
    int valor = _ret_sensores[i]; //Valor del sensor actual
    int v_rojo = v_rojos[i];
    int v_blanco  = v_blancos[i];
    int v_negro = v_negros[i];
    if ((valor >= v_rojo - umbral) && (valor < v_rojo + umbral)) { //Si es rojo
      _linea[i] = -1;
    }
    else if (valor >= v_rojo + umbral) { //Si es negro
      _linea[i] = 1;
    }
    else if (valor < v_rojo - umbral) { //Si no es blanco
      _linea[i] = 0;
    }
  }
  return true;
}
//-----------------------------------------------------------
/////////////////Funciones sensores/////////////////
/*int leer_ultrasonido() {
  //Función genérica que lee el valor de los sensores
  return distancia;

  }*/

int leer_ir(int n_pin) {
  //Toma el pin del analógico del sensor y retorna el valor de este
  return analogRead(n_pin);
}

bool leer_sensores(int _ret_sensores[n_sensores], int calibrar) {
  //Cambia el valor del array de los valores de los sensores según el sensor que deba,
  //El segundo parametro es si actualiza o no según la lista de desviaciónes del promedio
  //Por cada sensor
  for (int i = 0; i < n_sensores; i++) {
    int valor_ajuste = (calibrar == 1) ? constantes_calibrar[i] : 0;
    _ret_sensores[i] = leer_ir(sensores[i]) + valor_ajuste;
  }
  return true;
}

//-----------------------------------------------------------
/////////////////Funciones motor/////////////////
void stop_motors() {
  digitalWrite(pin_alimentacion_puente_h, LOW);
}
void move_motor_distance(double distance, int speed_L, int speed_R) {
  unsigned long initial_count = encoder_count_R;
  float perimeter = pi * diameter;
  int pulses = (distance * ratio * pulses_per_axis) / perimeter;
  Serial.println("___________________________________________");
  Serial.println(pulses);
  while ((encoder_count_R - initial_count) != pulses) {
    move_motors(speed_L, speed_R);
    Serial.println(encoder_count_R - initial_count);
  }
  move_motors(255, 255);
  delay(50);
}
void move_motors(int speed_L, int speed_R) {
  //Esta función según el signo de la velocidad hace avanzar o girar ambos motores independientemente
  bool dir_R  = (speed_R >= 0) ? 1 : 0;
  bool dir_L  = (speed_L >= 0) ? 1 : 0;
  if (dir_L)forward(1, speed_L);
  else backward(1, abs(speed_L));
  if (dir_R)forward(2, speed_R);
  else backward(2, abs(speed_R));
}
void forward(int motor, int _speed) {
  digitalWrite(pin_alimentacion_puente_h, HIGH);
  //Esta función hace avanzar un motor específico
  if (motor == 2) {//Derecho
    analogWrite(AIA, _speed); digitalWrite(AIB, LOW);
  }
  if (motor == 1) {//Izquierdo
    analogWrite(BIA, _speed); digitalWrite(BIB, LOW);
  }
}
void backward(int motor, int _speed) {
  digitalWrite(pin_alimentacion_puente_h, HIGH);
  //Esta función hace retroceder un motor específico
  if (motor == 2) {//Derecho
    digitalWrite(AIA, LOW); analogWrite(AIB, _speed);
  }
  if (motor == 1) {//Izquierdo
    digitalWrite(BIA, LOW); analogWrite(BIB, _speed);
  }
}
