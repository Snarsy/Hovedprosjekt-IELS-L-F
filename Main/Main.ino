#include <Zumo32U4.h>
#include <Wire.h>
#include <EEPROM.h>

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4OLED display;
Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensor;
Zumo32U4Buzzer buzzer;
Zumo32U4IMU imu;

// Menu:
int menuVar = 0;
int oneSecond = 1000;

// Speedometer:
int A = 1;
int16_t firstSpeed = 0;
unsigned long previousSpeedMillis = 0;
int readTime = 100;
int16_t totalSpeed = 0;
int speedDistance = 0;
int totalDistance = 0;
bool seventyMillis_start = false;
bool stoppedTimer = false;
unsigned long prevAveragesMillis = 0;
int holdTimerValue, secondsAboveSeventy, aboveSeventyCounter, maxSpeed, distanceAverage, averageSpeed60Sec = 0;


// Linefollower:
unsigned int lineSensorValues[5] = {0, 0, 0, 0, 0}; // 5 sensorer
const uint16_t followLinemaxSpeed = 300;
int16_t lastError = 0;
int Rspeed = 100;
int Lspeed = 100;
int lineFollowMenuVar = 0;
bool proxClear = false;

// Pattern
int patternVar = 0;

//bank account:
int bankAccountAddress = 1;
int bankAccount = EEPROM.read(bankAccountAddress);
int batteryCost = 0;
int batteryChangeCost = 150;


//software battery:
int serviceVar = 0;
int chargeVar = 0;
int batteryLife = 100;
int batteryFull = 1;
int batteryHealthAddress = 0;
int batteryHealthPercentage = EEPROM.read(batteryHealthAddress);
int updatedBatteryHealthPercentage = 0;
int chanceForBatteryMalfunction = 10000;
unsigned long batteryStatusPreviousMillis = 0;
unsigned long batteryDisplayPreviousMillis = 0;
unsigned long batteryDisplayMillis, batteryStatusMillis, batteryHealthMillis;
int intervalBatteryStatus = 10000;
int intervalBatteryDisplay = 1000;
int chargingCycle = 0;
int batteryStatusDisplayVar = 0;
bool tenPercentClear = false;
bool fivePercentageStop = false;
bool batteryCaseBlock = false;
unsigned long batteryHealthPreviousMillis = 0;


void setup()
{
    Serial.begin(9600);
    lineSensors.initFiveSensors();
    proxSensor.initFrontSensor();
    lineSensors.emittersOn();
    randomSeed(analogRead(0));
}

//////////////////////////SPEEDOMETER//////////////////////////////////
void speedometer() //Måler fart hvert 10.dels sekund. Siden readtime = 100.
{
    unsigned long speedMillis = millis();
    if (A == 1)
    {
        firstSpeed = encoder.getCountsLeft() + encoder.getCountsRight();
        A = 0;
    }
    if (speedMillis - previousSpeedMillis > readTime)
    {
        int16_t lastSpeed = encoder.getCountsLeft() + encoder.getCountsRight();
        A = 1;
        previousSpeedMillis = speedMillis;
        totalSpeed = abs((lastSpeed - firstSpeed) / 909.70 * 10.996 * 4); //Verdiene er regnet med hvor mange ganger den teller og areal av hjulet.
        speedDistance += totalSpeed/10; //Deler på 10 siden den teller hvert 1/10 sekund. 
        totalDistance += speedDistance;
        distanceAverage += totalSpeed/10;
        chanceForReductionOfBatteryHealth();   //Random faktoren for at batteriet mister 50%.
        maxSpeed70Percent(); 
    }
}

void maxSpeed70Percent(){ //Teller hvor mange ganger farten er over 70% av maxspeed.
    int aboveSeventySpeed = (maxSpeed * 0.7);

    if ((totalSpeed > aboveSeventySpeed)){
        aboveSeventyCounter += 1;
    }
}

void maxSpeed_measure(){ //Måler maxspeed
    if (totalSpeed > maxSpeed){
        maxSpeed = totalSpeed;
    }
}

void speedometerDisplay(){ //Displayer hastighet
        display.gotoXY(0, 5);
        display.print(F("Hastighet: "));
        display.gotoXY(10, 5);
        display.print(totalSpeed);
        display.gotoXY(12, 5);
        display.print(F("cm/s"));
}

void distanceDisplay(){
    int16_t distanceInMeters = totalDistance / 100;
    display.gotoXY(0,7);
    display.print(F("Distanse:"));
    display.gotoXY(10,7);
    display.print(distanceInMeters);
    display.gotoXY(14,7);
    display.print(F("m"));
}

void averagesInAMinute(){
    unsigned long averagesMillis = millis(); //AveragesMillis skal være lik millis hele tiden

    if (seventyMillis_start == false){ //Tiden skal starte samtidig, men prevAveragesMillis skal kun startes en gang
        prevAveragesMillis = millis();
        seventyMillis_start = true;
    }

    if (totalSpeed == 0){ //Dersom Zumo stopper skal den "holde" tida
        if (stoppedTimer == false){
            holdTimerValue += (averagesMillis - prevAveragesMillis);
            stoppedTimer = true;
        }
        prevAveragesMillis = averagesMillis;
    } else {
        stoppedTimer = false;
    }

    if ((averagesMillis - prevAveragesMillis) > (60000 - holdTimerValue)){
        secondsAboveSeventy = (aboveSeventyCounter/10); //Tar inn hvor mange ganger den har telt at farten er over 70% av maks, og deler på 10 fordi den teller 10 ganger i sekundet

        averageSpeed60Sec = distanceAverage/60; //Tar inn totaldistanse over ett minutt og deler på 60 sekund, som gir cm/s
        batteryHealthAfterAverageMeasure(); //Reduserer batterihelsen ut av infoen her

        distanceAverage = 0;
        aboveSeventyCounter = 0;
        maxSpeed = 0;

        prevAveragesMillis = averagesMillis;
        holdTimerValue = 0;
    }
} 


//////////////////////////LINEFOLLOWER/////////////////////////////////
void lineFollowMenu()
{
    switch (lineFollowMenuVar)
    {
    case 0:
        doYouWantToCalibrate();
        break;
    case 1:
        lineFollowMenuDisplay();
        break;
    case 2:
        driveLinePID();
        footInFront();
        statusDisplay();
        speedometer();
        batteryLevel();
        maxSpeed_measure();
        averagesInAMinute();
        break;
    case 3:
        driveLineStandard();
        footInFront();
        statusDisplay();
        speedometer();
        batteryLevel();
        maxSpeed_measure();
        averagesInAMinute();
        break;
    }
}

void doYouWantToCalibrate()
{
    aAndBFor();
    display.gotoXY(0, 0);
    display.print(F("Want to calibrate?"));
    
    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        calibrating();
        lineFollowMenuVar = 1;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        lineFollowMenuVar = 1;
    }
}

void lineFollowMenuDisplay()
{
    pressFor();
    display.gotoXY(0, 1);
    display.print(F("DriveLinePID"));
    display.gotoXY(0, 4);
    display.print(F("DriveLineStandard"));
    display.gotoXY(0, 7);
    display.print(F("Back"));
    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        delay(1000);
        lineFollowMenuVar = 2;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        delay(1000);
        lineFollowMenuVar = 3;
    }
    if (buttonC.getSingleDebouncedPress())
    {
        display.clear();
        lineFollowMenuVar = 0;
        menuVar = 0;
    }
}

void driveLinePID()
{
    int16_t position = lineSensors.readLine(lineSensorValues);

    //display.gotoXY(9,4);
    //display.print(position);
    int16_t error = position - 2000;
    int16_t speedDifference = error/0.5 + 4* (error - lastError);    

    lastError = error;

    int leftSpeed  = followLinemaxSpeed + speedDifference;
    int rightSpeed = followLinemaxSpeed - speedDifference;
    leftSpeed = constrain(leftSpeed, 0, (int16_t)followLinemaxSpeed);
    rightSpeed = constrain(rightSpeed, 0, (int16_t)followLinemaxSpeed);
    
    motors.setSpeeds(leftSpeed, rightSpeed);
}

void driveLineStandard()
{
    int Read = lineSensors.readLine(lineSensorValues);
    //display.gotoXY(9, 4);
    //display.print(Read);
    if (Read < 2000)
    {
        if (Read < 1750)
        {
            Lspeed = 100;
            Rspeed = 300;
        }
        else
        {
            Lspeed = 80;
            Rspeed = 120;
        }
    }
    else if (Read > 2000)
    {
        if (Read > 2250)
        {
            Lspeed = 300;
            Rspeed = 100;
        }
        else
        {
            Lspeed = 120;
            Rspeed = 80;
        }
    }

    motors.setSpeeds(Lspeed, Rspeed);
}

void calibrating()
{
    for (int i = 0; i < 60; i++)
    {
        motors.setSpeeds(200, -200);
        lineSensors.calibrate();
        delay(10);
    }
    display.clear();
    motors.setSpeeds(0, 0);
}

/////////////////////////Driving Pattern///////////////////////////////
int sjekk1;
int sjekk2;
int vinkel;
int rotation = 0;
int turns;
char fargetrykk = 'D';
unsigned long patternmillis;
#include "Turnsensor.h"
void turndeg(int tilverdi){ //87 grader er lik 90
    delay(5);
    sjekk1=0;
    sjekk2=0;
    tilverdi += 180;
    turnSensorReset();
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

void whatPattern(){
    pressFor();
    display.gotoXY(0, 1);
    display.print(F("Square"));
    display.gotoXY(0, 4);
    display.print(F("Circle"));
    display.gotoXY(0, 7);
    display.print(F("ForwardBackward"));
    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        patternVar = 2;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        patternVar = 3;
    }
    if (buttonC.getSingleDebouncedPress())
    {
        display.clear();
        patternVar = 4;
    }
}
void calibratepattern(){
    aAndBFor();
    display.gotoXY(0, 0);
    display.print(F("Want to calibrate?"));
    
    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        turnSensorSetup();
        patternVar = 1;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        patternVar = 1;
    }
}    

void Patternmenu()
{
    switch (patternVar)
    {
    case 0:
        calibratepattern();
        break;
    case 1:
        whatPattern();
        turnSensorReset();
        patternmillis = millis();
        turns = 0;
        break;
    case 2:
        squarePattern();
        statusDisplay();
        speedometer();
        batteryLevel();
        break;
    case 3:
        circlePattern();
        statusDisplay();
        speedometer();
        batteryLevel();
        break;
    case 4:
        forwardBackpattern();
        statusDisplay();
        speedometer();
        batteryLevel();
        break;
    }
    
}

void squarePattern(){
    if(millis()-patternmillis>200){
        if(millis()-patternmillis<1000 && turns<4){
        motors.setSpeeds(200,200);
    }
    else if(turns==4){
        patternVar=0;
        menuVar=0;
    }
    else{
        turndeg(87);
        patternmillis=millis()-200;
        turns++;
    }
    }         
}
void circlePattern(){
    if(millis()-patternmillis>500){
        motors.setSpeeds(200,100);
        turnSensorUpdate();
        vinkel = ((((int32_t)turnAngle >> 16) * -360) >> 16)+180;
        if (vinkel==175){
        motors.setSpeeds(0,0);
        patternVar=0;
        menuVar=0;
    }
    }
}
void forwardBackpattern(){
    if(millis()-patternmillis>500){
        motors.setSpeeds(200,200);
        if(millis()-patternmillis>2000){
            turndeg(175);
            patternmillis = millis()-500;
            turns++;
        }
        if (turns==2){
                patternVar=0;
                menuVar=0;
            }
    }
}

/////////////////////////PROX STOP/////////////////////////////////////
void footInFront()
{
    proxSensor.read();
    uint16_t frontleftPosition = proxSensor.countsFrontWithLeftLeds();
    uint16_t frontrightPosition = proxSensor.countsFrontWithRightLeds();

    if ((frontleftPosition == 6) || (frontrightPosition == 6))
    {
        motors.setSpeeds(0, 0);
        display.clear();
        menuVar = 5;
    }
}

void proxBackToMenu()
{
    if (proxClear == false){
        display.clear();
        proxClear = true;
    }
    display.gotoXY(0, 0);
    display.print(F("Back to menu?"));
    aAndBFor();
    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 0;
        proxClear = false;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 2;
        proxClear = false;
    }
}

//////////////////////////Software batteri/////////////////////////////
void batteryStatusTimer(){
    batteryStatusMillis = millis();
    if (batteryStatusMillis - batteryStatusPreviousMillis > intervalBatteryStatus){
        display.clear();
        batteryStatusDisplayVar = 1;
        batteryCaseBlock = true;
        batteryStatusPreviousMillis = batteryStatusMillis;
        batteryDisplayPreviousMillis = millis();
    }
}

void batteryDisplay(){
    batteryDisplayMillis = millis();
    display.gotoXY(0,0);
    display.print(F("BatteryLevel: "));
    display.gotoXY(0,1);
    display.print(batteryLife);
    display.gotoXY(0,3);
    display.print(F("Charging cycle: "));
    display.gotoXY(0,4);
    display.print(chargingCycle);
    display.gotoXY(0,6);
    display.print(F("BatteryHealth: "));
    display.gotoXY(0,7);
    display.print(batteryHealthPercentage);
    if (batteryDisplayMillis - batteryDisplayPreviousMillis > (intervalBatteryDisplay)){
        display.clear();
        batteryDisplayPreviousMillis = batteryDisplayMillis;
        batteryStatusDisplayVar = 0; 
        batteryCaseBlock = false;
        }
}

void statusDisplay(){
    switch (batteryStatusDisplayVar){
        case 0:
            speedometerDisplay();
            distanceDisplay();
            batteryLevelDisplay();
            batteryCase();
            batteryStatusTimer();
            batteryHealth();
            break;
        case 1:
            batteryDisplay();
            break;
        case 2: 
            lowBatteryDisplay(); 
            //batteryLevelDisplay();
            break;
        case 3: 
            chargeBatteryDisplay();
            //batteryLevelDisplay();
            break;
        case 4:
            emptyBatteryDisplay();
        case 5:
            batteryMalfunction();


    }
}

void emptyBatteryDisplay(){
    motors.setSpeeds(0,0); 
    display.clear();
    menuVar = 3;
    chargeVar = 1;
}

void chargeBatteryDisplay(){
    if (fivePercentageStop == false){
        display.clear();
        batteryHealthPercentage = batteryHealthPercentage - 5; //HVER GANG DEN GÅR TIL 5 PROSENT
        fivePercentageStop = true; //SIDEN JEG IKKE HAR LADESTASJON SÅ SPØR DEN MED EN GANG
    }
    motors.setSpeeds(0,0);

    menuVar = 3;
}

void lowBatteryDisplay(){
    if (tenPercentClear == false){
        display.clear();
        tenPercentClear = true;
    }

    display.gotoXY(5,4);
    display.print(F("LOW BATTERY"));

    buzzer.playFrequency(800, 1000, 10);

    if ((batteryLife < 10)){
        batteryStatusDisplayVar = 0;
        display.clear();
    }   
}

void batteryCase(){
    if (batteryCaseBlock == false){
        if (batteryLife > 100){
            batteryLife = 100;
        } else if (batteryLife > 10){
            batteryStatusDisplayVar = 0;
        } else if (batteryLife > 5){
            if (tenPercentClear == false){
                batteryStatusDisplayVar = 2;
        }
        } else if (batteryLife > 0){
            if (fivePercentageStop == false){
                batteryStatusDisplayVar = 3;
        }
        } else if (batteryLife == 0){
            batteryStatusDisplayVar = 4;
        }
        }
}



// Batteri nivå fra 0-100
void batteryLevel()
{
    if (speedDistance > 20){
        batteryLife -= 1;   
        speedDistance = 0;     
    }
}

void batteryLevelDisplay(){
    if (batteryLife == 100){
        display.gotoXY(18,0);
        display.print(batteryLife);
    } else{
        display.gotoXY(19,0);
        display.print(batteryLife);
    }
    if (batteryLife < 100 && batteryFull == 1){
        display.clear();
        batteryFull = 0;
    }

}



///////////////////////Helsetilstand////////////////////////////////////
// Batteri helse fra 0-100, der 100 er beste verdi
void batteryHealth()
{   
    if (batteryHealthPercentage < 0){
        batteryHealthPercentage = 0;
    }

    updateBatteryHealthEeprom();
    //chanceForReductionOfBatteryHealth();
}

void updateBatteryHealthEeprom(){
    EEPROM.write(batteryHealthAddress, batteryHealthPercentage);

}

void updateBankAccountEeprom(){
    EEPROM.write(bankAccountAddress, bankAccount);
}

void chanceForReductionOfBatteryHealth(){
    long RandomCheckBatteryMalfunction = random(1,chanceForBatteryMalfunction);
    if (RandomCheckBatteryMalfunction == 1){
        batteryHealthPercentage = batteryHealthPercentage/2;
        display.clear();
        batteryStatusDisplayVar = 5; 
        batteryHealthPreviousMillis = batteryHealthMillis;
    }
}

void batteryMalfunction(){
    batteryHealthMillis = millis();
    display.gotoXY(5,4);
    display.print(F("MALFUNCTION"));
    display.gotoXY(5,5);
    display.print(F("BATTERY HALVED"));

    buzzer.playNote(NOTE_A(4),500,15);

    if (batteryHealthMillis - batteryHealthPreviousMillis > (5 * oneSecond)){
        batteryStatusDisplayVar = 0;
        batteryHealthPreviousMillis = batteryHealthMillis;
        display.clear();
    }
}

void batteryHealthAfterAverageMeasure(){
    int batteryEffectSecondsAboveSeventy = secondsAboveSeventy*0.25;
    int batteryEffectAverageSpeed60Sec = averageSpeed60Sec*0.25;
    int batteryEffectMaxSpeed = maxSpeed * 0.05;
    int batteryEffectSum = (batteryEffectAverageSpeed60Sec + batteryEffectSecondsAboveSeventy) * batteryEffectMaxSpeed; 
    batteryHealthPercentage -= batteryEffectSum;
    //buzzer.playFrequency(250,1000,15);
}



// Hvis med ladestasjon, dukker dette displayet opp.
void chargingMenu()
{
    switch (chargeVar)
    {
    case 0:
        doYouWantToCharge();
        break;
    case 1:
        chargingOrBatteryService();
        batteryHealth();
        break;
    case 2:
        display.gotoXY(0, 0);
        display.print(F("Charging"));
        batteryLife = 100;
        if (buttonA.getSingleDebouncedPress()){
        batteryHealthPercentage = batteryHealthPercentage - 3;
        chargeVar = 1;
        chargingCycle += 1;
        }
        break;
    case 3:
        batteryServiceOrChangeMenu();
        break;
    }
}

void batteryChargingMenu(){
    display.gotoXY(0,0);
    display.print("How much do you wan to charge?");
    display.gotoXY(0,2);
    display.print("A for full battery");
    display.gotoXY(0,4);
    display.print("B for +25%");
    display.gotoXY(0,6);
    display.print("C for +10%");

    if (buttonA.getSingleDebouncedPress()){

    } 
}

void doYouWantToCharge()
{
    display.gotoXY(0, 0);
    display.print(F("Do you want to charge"));
    display.gotoXY(0, 3);
    display.print(F("A for YES"));
    display.gotoXY(12, 3);
    display.print(F("B for NO"));
    display.gotoXY(0,5);
    display.print("Bank account: ");
    display.gotoXY(18,5);
    display.print(bankAccount);
    display.gotoXY(0, 6);
    display.print(F("Battery percent "));
    display.gotoXY(18, 6);
    display.print(batteryLife);
    display.gotoXY(0, 7);
    display.print(F("Battery health "));
    display.gotoXY(18, 7);
    display.print(batteryHealthPercentage);

    if (buttonA.getSingleDebouncedPress())
    {
        chargeVar = 1;
        display.clear();
    }
    if (buttonB.getSingleDebouncedPress())
    {
        if (batteryStatusDisplayVar == 3){
            menuVar = 2;
            batteryStatusDisplayVar = 0;
            display.clear();
        }
        else {
            menuVar = 0;
            display.clear();
        }
    }
}

void chargingOrBatteryService()
{
    pressFor();
    display.gotoXY(0, 1);
    display.print(F("Charging"));
    display.gotoXY(0, 4);
    display.print(F("Battery service")); // service eller replacement
    display.gotoXY(0, 7);
    display.print(F("Back"));
    if (buttonA.getSingleDebouncedPress())
    {
        chargeVar = 2;
        display.clear();
    }
    if (buttonB.getSingleDebouncedPress())
    {
        chargeVar = 3;
        display.clear();
    }
    if (buttonC.getSingleDebouncedPress())
    {
        chargeVar = 0;
        display.clear();
    }
}

void batteryServiceOrChangeMenu()
{
    pressFor();
    display.gotoXY(0, 1);
    display.print(F("Change of battery"));
    display.gotoXY(0, 4);
    display.print(F("Battery service")); 
    display.gotoXY(0, 7);
    display.print(F("Back"));
    if (buttonA.getSingleDebouncedPress())
    {
        serviceVar = 1;
        menuVar = 6;
        display.clear();
    }
    if (buttonB.getSingleDebouncedPress())
    {
        serviceVar = 2;
        menuVar = 6;
        display.clear();
    }
    if (buttonC.getSingleDebouncedPress())
    {
        chargeVar = 1;
        display.clear();
    }
}

void batteryCharging()
{
}

void batteryHealthServiceCost(){
    if (batteryHealthPercentage <= 10){
        display.gotoXY(0,0);
        display.print("BatteryHealth too low");
        display.gotoXY(0,1);
        display.print("must change battery");
        display.gotoXY(0,3);
        display.print("transfering to menu");
    }
    if (batteryHealthPercentage >= 10){

    batteryCost = (100-batteryHealthPercentage) * 2;
    updatedBatteryHealthPercentage = batteryHealthPercentage + 20;
    }
    if (updatedBatteryHealthPercentage > 99){
        updatedBatteryHealthPercentage = 99;
    }
    serviceWillCostMenu();
    
    if (buttonA.getSingleDebouncedPress()){
     updateBankAccountEeprom(); 
     calibratePaymentBatteryService();
    }

    if (buttonB.getSingleDebouncedPress()){
        display.clear();
        menuVar = 0;
    }
}

void serviceWillCostMenu(){
    display.gotoXY(0,0);
    display.print("Service will cost:");
    display.gotoXY(18,0);
    display.print(batteryCost);
    display.gotoXY(0,1);
    display.print("Updated health:");
    display.gotoXY(18,1);
    display.print(updatedBatteryHealthPercentage);
    display.gotoXY(0,3);
    display.print("Do you want to pay?");
    display.gotoXY(0,5);
    display.print("A for YES");
    display.gotoXY(12,5);
    display.print("B for NO");
    display.gotoXY(0,7);
    display.print("Bank account:");
    display.gotoXY(14,7);
    display.print(bankAccount);

}

void batteryChange(){
    display.gotoXY(0,0);
    display.print("This will cost:");
    display.gotoXY(15,0);
    display.print(batteryChangeCost);
    display.gotoXY(0,1);
    display.print("Do you want to change?");
    display.gotoXY(0,3);
    display.print("A for YES");
    display.gotoXY(14,3);
    display.print("B for YES");
    display.gotoXY(0,7);
    display.print("Bank account:");
    display.gotoXY(14,7);
    
    if (buttonA.getSingleDebouncedPress()){
        if (bankAccount <= batteryChangeCost){

        }
    }
}

void calibratePaymentBatteryService(){
    display.clear();
    display.gotoXY(0,3);
    display.print("Calibrating payment...");
    bankAccount -= batteryCost;
    batteryHealthPercentage += 20; //fikser bare 20%
    delay(3000);
    display.clear();
    display.gotoXY(0,3);
    display.print("Payment done!");
    display.gotoXY(0,5);
    display.print("Returning to menu...");
    delay(3000);
    display.clear();
    menuVar = 0;
}

void batteryService(){
    switch (serviceVar)
    { 
    case 1:
        batteryChange();
        break;
    case 2:
        batteryHealthServiceCost();
        break;
    }
}


//////////////////MENU////////////////////////
void menu()
{
    switch (menuVar)
    {
    case 0:
        display.setLayout21x8();
        display.clear();
        lineFollowMenuVar = 0;
        chargeVar = 0;
        batteryStatusDisplayVar = 0;
        menuVar = 1;
        serviceVar = 0;
        break;
    case 1:
        menuDisplay1();
        break;
    case 2:
        lineFollowMenu();
        break;
    case 3:
        chargingMenu();
        break;
    case 4:
        Patternmenu();
        break;
    case 5:
        proxBackToMenu();
        speedometer();
        break;
    case 6:
        batteryService(); 
        break;
    case 7: 
        menuDisplay2();
        break;
    case 8:
        //work
        break;
    }
}

void pressFor(){
    display.gotoXY(0, 0);
    display.print(F("Press A for: "));
    display.gotoXY(0, 3);
    display.print(F("Press B for: "));
    display.gotoXY(0, 6);
    display.print(F("Press C for: "));

}

void aAndBFor(){
    display.gotoXY(0, 3);
    display.print(F("A for YES"));
    display.gotoXY(12, 3);
    display.print(F("B for NO"));
}

void menuDisplay1()
{
    pressFor();
    display.gotoXY(0, 1);
    display.print(F("lineFollow"));
    display.gotoXY(0, 4);
    display.print(F("ChargingMenu"));
    display.gotoXY(0, 7);
    display.print(F("Next"));
    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 2;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 3;
    }
    if (buttonC.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 7;
    }
}

void menuDisplay2(){
    pressFor();
    display.gotoXY(0, 1);
    display.print(F("Work"));
    display.gotoXY(0, 4);
    display.print(F("Drivingpattern"));
    display.gotoXY(0, 7);
    display.print(F("Next"));
     
    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 8;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 4;
    }
    if (buttonC.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 0;
    }
}

void loop()
{
    menu();
    
}
