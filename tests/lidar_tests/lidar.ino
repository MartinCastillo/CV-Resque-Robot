#  include "VL53L0X.h"
#define LONG_RANGE
VL53L0X sensor;
#define HIGH_SPEED
#  include <Wire.h>

int pines_stepper[4] = {11, 10, 9, 8};
boolean direction = true;
/*
  int Paso =
  { {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
  };*/
int Paso[4][4] =
{ {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

//Se le multiplica
const int stepsPerRevolution = (32 * 64) * ((sizeof(Paso) / sizeof(Paso[0])) / 4);
const int vueltas = 1;
int Steps = 0;
int base_radius = 30;//mm
void setup() {
  for (int i = 0 ; i <= 4 ; i++) {
    pinMode(pines_stepper[i], OUTPUT);
  }
  Serial.begin(9600);
  Wire.begin();

  sensor.init();
  sensor.setTimeout(250);

  sensor.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

  sensor.setMeasurementTimingBudget(20000);
}

void loop() {
  for (int i = 0; i <= stepsPerRevolution * vueltas ; i++) {
    stepper();
    /////////
    int distancemm = sensor.readRangeSingleMillimeters();
    Serial.println(distancemm+base_radius);
  }
  direction = !direction;
  delay(1);
}

void stepper() {
  for (int i = 0 ; i <= 4 ; i++) {
    digitalWrite(pines_stepper[i], Paso[Steps][i]);
  }
  if (direction)
    Steps ++;
  else
    Steps --;
  int pasos_por_ciclo = sizeof(Paso) / sizeof(Paso[0]);
  Steps = ((Steps +  pasos_por_ciclo ) % pasos_por_ciclo );
}
