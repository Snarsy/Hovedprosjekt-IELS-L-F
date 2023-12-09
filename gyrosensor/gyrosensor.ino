#include <Wire.h>
#include <Zumo32U4.h>


Zumo32U4OLED display;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4IMU imu;
Zumo32U4Buzzer buzzer;


#include "Turnsensor.h"

void setup(){
    gyroCalibrating();
}

void gyroCalibrating(){
    turnSensorSetup();
    delay(500);
    turnSensorReset();
}


void loop(){
    //turnSensorUpdate();
    
    imu.readGyro();
    int angle = imu.getType();
    // Read gyroscope data
  int angle = turnAngle;

  // Map gyroscope data to angle ranges
  if ((angle >= 0) && (angle <= 90)) {
    // Play buzzer sound 1
    buzzer.playFrequency(1000, 200,10); // Adjust frequency and duration as needed
  } else if ((angle > 90) && (angle <= 180)) {
    // Play buzzer sound 2
    buzzer.playFrequency(1500, 200,10);
  } else if ((angle > 180) && (angle <= 270)) {
    // Play buzzer sound 3
    buzzer.playFrequency(2000, 200,10);
  } else if ((angle > 270) && (angle <= 360)) {
    // Play buzzer sound 4
    buzzer.playFrequency(2500, 200,10);
  }

  delay(100); // Adjust delay as needed
}
