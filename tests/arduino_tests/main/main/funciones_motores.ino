/*
Funciones dedicadas al control de los motores
*/
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
