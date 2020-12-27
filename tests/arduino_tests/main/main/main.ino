//https://forum.arduino.cc/index.php?topic=483350.0
//////////////////////////////////// Global Variables /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
/////////////////Arrays sensores/////////////////
//El indice 0 es el del sensor de la izquierda
const int n_sensores = 5;
int sensores[n_sensores] = {0, 1, 2, 3, 4};
/*Una constante obtenida del promedio de los valores de los sensores frente a una misma condición
  En nuevos entornos reasignar estas variables*/
int constantes_calibrar[n_sensores] = { 0, 0, 0, 0, 0};
int ret_sensores[n_sensores] = { 0, 0, 0, 0, 0};
int linea[n_sensores] = { 0, 0, 0, 0, 0};
/////////////////Umabrales sensores/////////////////
int v_blancos[n_sensores] = {50, 53, 51, 54, 60};
int v_rojos[n_sensores] = {200, 200, 200, 200, 200};
int v_negros[n_sensores] = {585, 550, 497, 557, 654};
int umbral = 7;

/////////////////Pines motores/////////////////
//http://www.bajdi.com/l9110-h-bridge-module/
const int AIA = 5;  // (pwm) pin A-IA Velocidad
const int AIB = 6;  // (pwm) pin A-IB Direción
const int BIA = 11; // (pwm) pin B-IA  Velocidad
const int BIB = 10;  // (pwm)pin B-IB Dirección
/////////////////Velocidades motores/////////////////
int speed_Iz = 0;
int speed_Ri = 0;
int max_vel = 160;

//valida diferentes partes del codigo según la necesidad, calibrar,
//principal, testear, etc
bool modo = 1;
//Para girar con señales rojas y para los GAP
bool girar_con_prox_negro = 0;
int  last_action = 0;
int direccion_giro_rojo;

/////////Constantes encoders, mediciones y variables volatiles/////////
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
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

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
