#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4ButtonA buttonA; 
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4OLED display;
Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensor;
Zumo32U4IMU imu;

#include "Turnsensor.h"

char fargetrykk = 'D';
unsigned long gammeltid = 0;
int rotation = 0;
int vinkel;
bool sjekk1=0;
bool sjekk2=0;

void showsplay(){
    display.gotoXY(0,0);
    display.print(vinkel);
}

void turndeg(int tilverdi){ //87 grader er lik 90
    sjekk1=0;
    sjekk2=0;
    tilverdi += 180;
    turnSensorReset();
    showsplay();
    while (vinkel<=tilverdi || vinkel>=tilverdi){
        turnSensorUpdate();
        vinkel = ((((int32_t)turnAngle >> 16) * -360) >> 16)+180;
        if (vinkel<tilverdi){
            sjekk1=1;
            motors.setSpeeds(100,-100);
        }
        else if (vinkel>tilverdi){
            sjekk2=1;
            motors.setSpeeds(-100,100);
        }
        if(sjekk1 && sjekk2){
            motors.setSpeeds(0,0);
            break;
        }
    }
    //display.print("Heeelo");
}
unsigned long prevmillis;
void fargeswitch(){
    while(fargetrykk=='D'){
        if (buttonA.isPressed()){fargetrykk='A';}
        else if (buttonB.isPressed()){fargetrykk='B';}
        else if (buttonC.isPressed()){fargetrykk='C';}
        prevmillis = millis();
        turnSensorReset();
    }
    if (millis()-prevmillis>1000){
    switch (fargetrykk){
        case 'A':
            for(int i=0;i<4;i++){
                motors.setSpeeds(200,200);
                delay(1000);
                turndeg(90);
            }
            fargetrykk='D';
            break;
        case 'B':
            motors.setSpeeds(200,100);
            turnSensorUpdate();
            vinkel = ((((int32_t)turnAngle >> 16) * -360) >> 16)+180;
            if (vinkel==179){
                motors.setSpeeds(0,0);
                fargetrykk='D';
            }
            break;
    }
    }
}

void setup(){
    turnSensorSetup();
    turnSensorReset();
}
void loop(){
    fargeswitch();
    
}