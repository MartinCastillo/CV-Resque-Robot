/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */
//#define _PARTICLE_BUILD_IDE_
#if defined(_PARTICLE_BUILD_IDE_)
#  include "VL53L0X/VL53L0X.h"
#else
#  include "VL53L0X.h"
#endif

#if !defined(PARTICLE)
#  include <Wire.h>
#endif


VL53L0X sensor;

#define LONG_RANGE

// Uncomment ONE of these two lines to get
// - higher speed at the cost of lower accuracy OR
// - higher accuracy at the cost of lower speed

#define HIGH_SPEED
//#define HIGH_ACCURACY

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  sensor.init();
  sensor.setTimeout(250);

 
 #if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(200000); 
#endif

}

void loop()
{

int distancemm = sensor.readRangeSingleMillimeters();

if (distancemm >= 9000)  //Change 9000 to the maximum distance you want to get readings from. Only readings below this number will serial print. 
{
  // action A
}
else
{
  Serial.println(distancemm);
}
}
