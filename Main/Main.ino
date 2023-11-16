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

//Menu:
int menuVar = 0;

//Speedometer:
int A = 1;
int16_t firstSpeed = 0;
unsigned long previousSpeedMillis = 0;
int readTime = 100;

//Linefollower:
unsigned int lineSensorValues[5] = {0,0,0,0,0}; //5 sensorer
const uint16_t followLinemaxSpeed = 300;
int16_t lastError = 0;
int Rspeed = 100;
int Lspeed = 100;
int lineFollowMenuVar = 0;

//Ladestasjon:
int chargeVar = 0;
int batteryLife = 100;
int batteryHealthPercentage = 99;




//////////////////////////SPEEDOMETER//////////////////////////////////
void speedometer(){
    unsigned long speedMillis = millis();
    if (A == 1){
        firstSpeed = encoder.getCountsLeft() + encoder.getCountsRight();
        A = 0;
    }
    if (speedMillis - previousSpeedMillis > readTime){
        int16_t lastSpeed = encoder.getCountsLeft() + encoder.getCountsRight();
        A = 1;
        previousSpeedMillis = speedMillis;
        int16_t totalSpeed = abs((lastSpeed-firstSpeed)/909.70*10.996*4);
        display.gotoXY(0,0);
        display.print(F("Hastighet: "));
        display.gotoXY(0,4);
        display.print(totalSpeed);
    }
}

//////////////////////////LINEFOLLOWER/////////////////////////////////
void lineFollowMenu(){
    switch (lineFollowMenuVar){
        case 0:
            doYouWantToCalibrate();
            break;
        case 1:
            lineFollowMenuDisplay();
            break;
        case 2:
            driveLinePID();
            break;
        case 3:
            driveLineStandard();
            break;

    }
}

void doYouWantToCalibrate(){
    display.gotoXY(0,0);
    display.print(F("Want to calibrate?"));
    display.gotoXY(0,3); 
    display.print(F("A for YES"));
    display.gotoXY(12,3);
    display.print(F("B for NO"));
    if (buttonA.getSingleDebouncedPress()){
        display.clear();
        calibrating();
        lineFollowMenuVar = 1;
    }
    if (buttonB.getSingleDebouncedPress()){
        display.clear();
        lineFollowMenuVar = 1;
    }
}

void lineFollowMenuDisplay(){
    display.gotoXY(0,0);
    display.print(F("Press A for: "));
    display.gotoXY(0,1);
    display.print(F("DriveLinePID"));
    display.gotoXY(0,3);
    display.print(F("Press B for: "));
    display.gotoXY(0,4);
    display.print(F("DriveLineStandard"));
    display.gotoXY(0,6);
    display.print(F("Press C for: "));
    display.gotoXY(0,7);
    display.print(F("Back"));
    if (buttonA.getSingleDebouncedPress()){
        display.clear();
        lineFollowMenuVar = 2;
    }
    if (buttonB.getSingleDebouncedPress()){
        display.clear();
        lineFollowMenuVar = 3;
    }
    if (buttonC.getSingleDebouncedPress()){
        display.clear();
        lineFollowMenuVar = 0;
        menuVar = 0;

    }
}

void driveLinePID(){
    
    int16_t position = lineSensors.readLine(lineSensorValues);

    display.gotoXY(9,4);
    display.print(position);
    int16_t error = position - 2000;
    int16_t speedDifference = error/4 + 5 * (error - lastError);    

    lastError = error;

    int leftSpeed  = followLinemaxSpeed + speedDifference;
    int rightSpeed = followLinemaxSpeed - speedDifference;
    leftSpeed = constrain(leftSpeed, 0, (int16_t)followLinemaxSpeed);
    rightSpeed = constrain(rightSpeed, 0, (int16_t)followLinemaxSpeed);
    
    motors.setSpeeds(leftSpeed, rightSpeed);
}

void driveLineStandard(){
    int Read = lineSensors.readLine(lineSensorValues);
    display.gotoXY(0, 0);
    display.print(Read);
    if (Read<2000){
        if (Read<1750){
            Lspeed=100;
            Rspeed=300;
        }
        else{
            Lspeed=80;
            Rspeed=120;
        }
    }
    else if (Read>2000){
        if (Read>2250){
            Lspeed=300;
            Rspeed=100;
        }
        else{
            Lspeed=120;
            Rspeed=80;
        }
    }

    motors.setSpeeds(Lspeed,Rspeed);
}

void calibrating(){
    for (int i = 0; i < 120; i++){
        motors.setSpeeds(200,-200); 
        lineSensors.calibrate();
        delay(10);
    }
    display.clear();
    motors.setSpeeds(0,0);
}

//////////////////////////Software batteri/////////////////////////////
/*Hvert 10. sekund skal dette displayes i 1. sekund: 
i. battery_level - Ladeprosenten for batteriet (0-100%)
ii. charging_cycles - Ant. ganger batteriet har blitt ladet
iii. battery_health - Helsetilstanden til batteriet. Se pkt. nedenfor
*/
void batteryStatus(){   

}

//Batteri nivå fra 0-100
void batteryLevel(){

}

//hvor mange ganger bilen har ladet
void chargingCycles(){

}

/*Kan bare benyttes en gang
Etter knappetrykk skal den lade 10x raskere når den rygges. Opp til 20%
*/
void EmergencyCharging(){

}

void backwardsCharging(){

}

/*
Batteri meny skal ha funksjon av display:
Batterystatus
Emergency charging
Angre funksjon
*/
void batteryMenu(){

}

///////////////////////Helsetilstand////////////////////////////////////
//Batteri helse fra 0-100, der 100 er beste verdi
void batteryHealth(){

}


///////////////Ladestasjon///////////////////////////////////
//Når den er på fem prosent går den sakte men sikkert mot ladestasjon

void goToCharging(){

}

//Hvis med ladestasjon, dukker dette displayet opp. 
void chargingMenu(){
    switch(chargeVar){
        case 0:
        doYouWantToCharge();
            break;
        case 1:
        chargingOrBatteryService();
            break;
        case 2:
            display.gotoXY(0,0);
            display.print(F("Charging"));
            break;
        case 3:
            display.gotoXY(0,0);
            display.print(F("Battery service"));
            break;
        case 4:
            display.gotoXY(0,0);
            display.print(F("no charging"));
            break;

    }
    
}

void doYouWantToCharge(){
    display.gotoXY(0,0);
    display.print(F("Do you want to charge"));
    display.gotoXY(0,3); 
    display.print(F("A for YES"));
    display.gotoXY(12,3);
    display.print(F("B for NO"));
    display.gotoXY(0,6);
    display.print(F("Battery percent "));
    display.gotoXY(18,6);
    display.print(batteryLife);
    display.gotoXY(0,7);
    display.print(F("Battery health "));
    display.gotoXY(18,7);
    display.print(batteryHealthPercentage);

    if (buttonA.getSingleDebouncedPress()){
        chargeVar = 1;
        display.clear();
    }    
    if (buttonB.getSingleDebouncedPress()){
        chargeVar = 4;
        display.clear();
    }
}

void chargingOrBatteryService(){
    display.gotoXY(0,0);
    display.print(F("Press A for: "));
    display.gotoXY(0,1);
    display.print(F("Charging"));
    display.gotoXY(0,3);
    display.print(F("Press B for: "));
    display.gotoXY(0,4);
    display.print(F("Battery service")); //service eller replacement
    display.gotoXY(0,6);
    display.print(F("Press C for: "));
    display.gotoXY(0,7);
    display.print(F("Back"));
            if (buttonA.getSingleDebouncedPress()){
                chargeVar = 2;
                display.clear();
            }
            if (buttonB.getSingleDebouncedPress()){
                chargeVar = 3;
                display.clear();
            }
            if (buttonC.getSingleDebouncedPress()){
                chargeVar = 0;
                display.clear();
            }
}

void batteryService(){

}

void batteryCharging(){

}

/*
To charge press A
For battery service/replacement press b
Cancel
*/

/*
Gamble all or nothing. Either free or you lose everything
Pay
cancel
*/

/*
Forskjellige lademuligheter:
Full
Vi velger prosent
Lading til vi trykker på stopp
Gambling av penger :)
*/


//////////////////MENU////////////////////////
void menu(){
    switch (menuVar){
        case 0:
            display.setLayout21x8();
            display.clear();
            menuVar = 1;
            break;
        case 1:
            menuDisplay(); 
            break;
        case 2:
            lineFollowMenu();
            break;
        case 3:
            chargingMenu(); 
            break;
        case 4:
            // ?
            break;
    }
    //ChargingMenu

}

void menuDisplay(){
    display.gotoXY(0,0);
    display.print(F("Press A for: "));
    display.gotoXY(0,1);
    display.print(F("lineFollow"));
    display.gotoXY(0,3);
    display.print(F("Press B for: "));
    display.gotoXY(0,4);
    display.print(F("ChargingMenu"));
    display.gotoXY(0,6);
    display.print(F("Press C for: "));
    display.gotoXY(0,7);
    display.print(F("?"));
    if (buttonA.getSingleDebouncedPress()){
        display.clear();
        menuVar = 2;
    }
    if (buttonB.getSingleDebouncedPress()){
        display.clear();
        menuVar = 3;
    }
    if (buttonC.getSingleDebouncedPress()){
        display.clear();
        menuVar = 4;
    }
}

void setup(){
    Serial.begin(9600);
    lineSensors.initFiveSensors();
    proxSensor.initThreeSensors();
    lineSensors.emittersOn();
}

void loop(){
    menu();
}
