/******************************************************************************
SparkFun 74HC4051 Multiplexer Analog Input
(https://www.sparkfun.com/products/13906)
https://github.com/sparkfun/74HC4051_8-Channel_Mux_Breakout
Mux Breakout ----------- Arduino
     S0 ------------------- 2
     S1 ------------------- 3
     S2 ------------------- 4
     Z -------------------- A0
    VCC ------------------- 5V
    GND ------------------- GND
    (VEE should be connected to GND)
******************************************************************************/
const int selectPins[3] = {2, 3, 4}; // S0~2, S1~3, S2~4
const int zInput = A0; // Connect common (Z) to A0 (analog input)
void setup() 
{
  Serial.begin(9600); // Initialize the serial port
  // Set up the select pins as outputs:
  for (int i=0; i<3; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  pinMode(zInput, INPUT); // Set up Z as an input
}

void loop() 
{

}

// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7.
int readMuxPin(byte pin){
  for (int i=0; i<3; i++){
    if (pin & (1<<i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
  return(analogRead(A0));
}
