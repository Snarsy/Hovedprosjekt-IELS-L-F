# 1 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
# 2 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 2
# 3 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 2

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
        int16_t totalSpeed = (((lastSpeed-firstSpeed)/909.70*10.996*4)>0?((lastSpeed-firstSpeed)/909.70*10.996*4):-((lastSpeed-firstSpeed)/909.70*10.996*4));
        display.gotoXY(0,0);
        display.print((reinterpret_cast<const __FlashStringHelper *>(
# 51 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                     (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 51 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                     "Hastighet: "
# 51 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                     ); &__c[0];}))
# 51 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                     )));
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
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 78 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 78 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Want to calibrate?"
# 78 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 78 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,3);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 80 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 80 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "A for YES"
# 80 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 80 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(12,3);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 82 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 82 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "B for NO"
# 82 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 82 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
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
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 96 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 96 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press A for: "
# 96 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 96 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,1);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 98 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 98 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "DriveLinePID"
# 98 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 98 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,3);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 100 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 100 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press B for: "
# 100 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 100 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,4);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 102 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 102 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "DriveLineStandard"
# 102 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 102 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,6);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 104 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 104 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press C for: "
# 104 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 104 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,7);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 106 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 106 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Back"
# 106 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 106 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
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

    int leftSpeed = followLinemaxSpeed + speedDifference;
    int rightSpeed = followLinemaxSpeed - speedDifference;
    leftSpeed = ((leftSpeed)<(0)?(0):((leftSpeed)>((int16_t)followLinemaxSpeed)?((int16_t)followLinemaxSpeed):(leftSpeed)));
    rightSpeed = ((rightSpeed)<(0)?(0):((rightSpeed)>((int16_t)followLinemaxSpeed)?((int16_t)followLinemaxSpeed):(rightSpeed)));

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
# 186 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
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
# 203 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
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
# 217 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
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
            display.print((reinterpret_cast<const __FlashStringHelper *>(
# 246 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                         (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 246 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                         "Charging"
# 246 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                         ); &__c[0];}))
# 246 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                         )));
            break;
        case 3:
            display.gotoXY(0,0);
            display.print((reinterpret_cast<const __FlashStringHelper *>(
# 250 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                         (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 250 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                         "Battery service"
# 250 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                         ); &__c[0];}))
# 250 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                         )));
            break;
        case 4:
            display.gotoXY(0,0);
            display.print((reinterpret_cast<const __FlashStringHelper *>(
# 254 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                         (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 254 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                         "no charging"
# 254 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                         ); &__c[0];}))
# 254 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                         )));
            break;

    }

}

void doYouWantToCharge(){
    display.gotoXY(0,0);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 263 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 263 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Do you want to charge"
# 263 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 263 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,3);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 265 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 265 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "A for YES"
# 265 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 265 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(12,3);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 267 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 267 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "B for NO"
# 267 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 267 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,6);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 269 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 269 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Battery percent "
# 269 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 269 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(18,6);
    display.print(batteryLife);
    display.gotoXY(0,7);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 273 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 273 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Battery health "
# 273 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 273 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
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
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 289 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 289 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press A for: "
# 289 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 289 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,1);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 291 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 291 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Charging"
# 291 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 291 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,3);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 293 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 293 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press B for: "
# 293 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 293 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,4);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 295 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 295 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Battery service"
# 295 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 295 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 ))); //service eller replacement
    display.gotoXY(0,6);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 297 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 297 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press C for: "
# 297 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 297 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,7);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 299 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 299 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Back"
# 299 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 299 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
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
# 328 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
/*

Gamble all or nothing. Either free or you lose everything

Pay

cancel

*/
# 334 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
/*

Forskjellige lademuligheter:

Full

Vi velger prosent

Lading til vi trykker på stopp

Gambling av penger :)

*/
# 343 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
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
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 370 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 370 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press A for: "
# 370 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 370 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,1);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 372 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 372 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "lineFollow"
# 372 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 372 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,3);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 374 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 374 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press B for: "
# 374 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 374 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,4);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 376 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 376 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "ChargingMenu"
# 376 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 376 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,6);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 378 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 378 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "Press C for: "
# 378 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 378 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
    display.gotoXY(0,7);
    display.print((reinterpret_cast<const __FlashStringHelper *>(
# 380 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 (__extension__({static const char __c[] __attribute__((__progmem__)) = (
# 380 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 "?"
# 380 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino" 3
                 ); &__c[0];}))
# 380 "C:\\Users\\larsu\\Desktop\\Skolearbeid\\IELS1001(ESI sem1)\\Arduino\\Hovedprosjekt-IELS-L-F\\Main\\Main.ino"
                 )));
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
