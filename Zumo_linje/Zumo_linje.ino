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

char fargetrykk = 'D';
unsigned long gammeltid = 0;

void knapp(){
    while(fargetrykk=='D'){
        if (buttonA.isPressed()){fargetrykk='A';}
        else if (buttonB.isPressed()){fargetrykk='B';}
        else if (buttonB.isPressed()){fargetrykk='C';}
    }
    delay(100);
    switch (fargetrykk){
        case 'A':
            gammeltid=millis();
            motors.setSpeeds(200,200);
            for (int i=0;i<4;i){
                if(millis()-gammeltid>762){
                    motors.setSpeeds(200,200);
                    gammeltid=millis();
                    i++;
                }
                else if(millis()-gammeltid>(662)){
                    motors.setSpeeds(0,0);
                }
                else if(millis()-gammeltid>500){
                    motors.setSpeeds(-400,400);
                }
                else if(millis()-gammeltid>400){
                    motors.setSpeeds(0,0);
                }
            }
            motors.setSpeeds(0,0);
            fargetrykk='D';
    }
}

void setup(){

}
void loop(){
    knapp();
}