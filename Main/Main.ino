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
unsigned long previousSpeedMillis, prevAveragesMillis = 0;
int speedDistance, totalDistance = 0;
int A = 1;
int16_t firstSpeed = 0;
int readTime = 100;
int16_t totalSpeed = 0;
int16_t negativeTotalSpeed = 0;
bool seventyMillis_start, stoppedTimer = false;
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

// bank account:
int bankAccountAddress = 1;
int bankAccount = EEPROM.read(bankAccountAddress);
int batteryCost = 0;
int batteryChangeCost = 150;

// software battery:
int serviceVar, chargeVar, chargingCycle, batteryStatusDisplayVar = 0;
int batteryLife = 100;
int batteryFull = 1;
int batteryHealthLevel = 0;
int batteryHealthAddress = 0;
int batteryHealthPercentage = EEPROM.read(batteryHealthAddress);
int updatedBatteryHealthPercentage = 0;
int chanceForBatteryMalfunction = 10000;
unsigned long batteryDisplayPreviousMillis, batteryStatusPreviousMillis, batteryHealthPreviousMillis = 0;
unsigned long batteryDisplayMillis, batteryStatusMillis, batteryHealthMillis;
int intervalBatteryStatus = 10000;
int intervalBatteryDisplay = 1000;
bool tenPercentClear, fivePercentageStop, batteryCaseBlock = false;

void setup()
{
    Serial.begin(9600);
    lineSensors.initFiveSensors();
    proxSensor.initFrontSensor();
    lineSensors.emittersOn();
    randomSeed(analogRead(0));
}

//////////////////////////SPEEDOMETER//////////////////////////////////
int secretCount = 0;
bool hiddenFeature, superHiddenFeature = false;
unsigned long prevHiddenFeatureMillis = 0;
int hiddenFeatureInterval = 120000;

void speedometer() // Måler fart hvert 10.dels sekund. Siden readtime = 100.
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
        totalSpeed = abs((lastSpeed - firstSpeed) / 909.70 * 10.996 * 4); // Verdiene er regnet med hvor mange ganger den teller og areal av hjulet.
        negativeTotalSpeed = totalSpeed * -1;
        speedDistance += totalSpeed / 10; // Deler på 10 siden den teller hvert 1/10 sekund.
        totalDistance += speedDistance;
        distanceAverage += totalSpeed / 10;
        chanceForReductionOfBatteryHealth(); // Random faktoren for at batteriet mister 50%.
        maxSpeed70Percent();
    }
}

void maxSpeed70Percent()
{ // Teller hvor mange ganger farten er over 70% av maxspeed.
    int aboveSeventySpeed = (maxSpeed * 0.7);

    if ((totalSpeed > aboveSeventySpeed))
    {
        aboveSeventyCounter += 1;
    }
}

void maxSpeed_measure()
{ // Måler maxspeed
    if (totalSpeed > maxSpeed)
    {
        maxSpeed = totalSpeed;
    }
}

void speedometerDisplay()
{ // Displayer hastighet
    display.gotoXY(0, 5);
    display.print(F("Hastighet: "));
    display.gotoXY(10, 5);
    display.print(totalSpeed);
    display.gotoXY(12, 5);
    display.print(F("cm/s"));
}

void distanceDisplay()
{ //Displayer distanse
    int16_t distanceInMeters = totalDistance / 100;
    display.gotoXY(0, 7);
    display.print(F("Distanse:"));
    display.gotoXY(10, 7);
    display.print(distanceInMeters);
    display.gotoXY(14, 7);
    display.print(F("m"));
}

void averagesInAMinute()
{ //Teller gjennomsnittshastighet, maxspeed og antall ganger over 70% på 1 minutt.
    unsigned long averagesMillis = millis(); // AveragesMillis skal være lik millis hele tiden

    if (seventyMillis_start == false)
    { // Tiden skal starte samtidig, men prevAveragesMillis skal kun startes en gang
        prevAveragesMillis = millis();
        seventyMillis_start = true;
    }

    if (totalSpeed == 0)
    { // Dersom Zumo stopper skal den "holde" tida
        if (stoppedTimer == false)
        {
            holdTimerValue += (averagesMillis - prevAveragesMillis);
            stoppedTimer = true;
        }
        prevAveragesMillis = averagesMillis;
    }
    else
    {
        stoppedTimer = false;
    }

    if ((averagesMillis - prevAveragesMillis) > (60000 - holdTimerValue))
    {
        secondsAboveSeventy = (aboveSeventyCounter / 10); // Tar inn hvor mange ganger den har telt at farten er over 70% av maks, og deler på 10 fordi den teller 10 ganger i sekundet

        averageSpeed60Sec = distanceAverage / 60; // Tar inn totaldistanse over ett minutt og deler på 60 sekund, som gir cm/s
        batteryHealthAfterAverageMeasure();       // Reduserer batterihelsen ut av infoen her

        distanceAverage = 0;
        aboveSeventyCounter = 0;
        maxSpeed = 0;

        prevAveragesMillis = averagesMillis;
        holdTimerValue = 0;
    }
}

//////////////////////////LINEFOLLOWER/////////////////////////////////
void lineFollowMenu()
{ //LinefollowMenuSwitchCase
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
        batteryHealthLevel0Or1();
        break;
    case 3:
        driveLineStandard();
        footInFront();
        statusDisplay();
        speedometer();
        batteryLevel();
        maxSpeed_measure();
        averagesInAMinute();
        batteryHealthLevel0Or1();
        break;
    }
}

void doYouWantToCalibrate()
{ //Spør om man vil calibrate
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
{ //Display for valg av linjefølging
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
{ //Linjefølging tatt fra Kevin Pololu. 
    int16_t position = lineSensors.readLine(lineSensorValues); //Leser linjeesensor. 
    int16_t error = position - 2000; //Finnet ut om det er venstre eller høyre som er mer utenfor linjen.
    int16_t speedDifference = error / 1 + 4 * (error - lastError); 

    lastError = error;

    int leftSpeed = followLinemaxSpeed + speedDifference;
    int rightSpeed = followLinemaxSpeed - speedDifference;
    leftSpeed = constrain(leftSpeed, 0, (int16_t)followLinemaxSpeed); //Constrain for å ikke få minus, og ikke over 400 verdi. 
    rightSpeed = constrain(rightSpeed, 0, (int16_t)followLinemaxSpeed);

    motors.setSpeeds(leftSpeed, rightSpeed);
}

void driveLineStandard()
{ //Linjefølging der den ser hvilke side som er mer utenfor linje, og så gir meg motorkraft til den motoren som er utenfor.
    int Read = lineSensors.readLine(lineSensorValues);
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
{ //Kalibrerer linjesensor
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
//For hele driving pattern har vi brukt "Turnsensor.h" for å få verdier fra gyroen. Denne har vi hentet fra zumo eksempelet rotationresist funnet i githubet:
//https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples/RotationResist.
//Fra denne har vi brukt turnsensorreset, turnsensorupdate og turnangle.

int sjekk1;
int sjekk2;
int vinkel;
int rotation = 0;
int turns;
unsigned long patternmillis;
#include "Turnsensor.h"
void turndeg(int tilverdi)//Denne funksjonen roterer bilen i 90 grader ved hjelp av gyroen. Den har 87 grader ettersom dette er hva 90 grader er lik på skolens gulv.
//Derimot med mer friksjon så vil man bruke 90 grader istedenfor 87 for 90.
//Denne er blokkerende, men det er grunnet at dette er en rotasjon og tilnærmet null batteriliv skal gå ned av dette.
{ // 87 grader er lik 90
  // Snur den 90 grader. 
    delay(5);
    sjekk1 = 0;
    sjekk2 = 0;
    tilverdi += 180;
    turnSensorReset();
    while (vinkel <= tilverdi || vinkel >= tilverdi) 
    {
        turnSensorUpdate();
        vinkel = ((((int32_t)turnAngle >> 16) * -360) >> 16) + 180;
        if (vinkel < tilverdi)
        {
            sjekk1 = 1;
            motors.setSpeeds(100, -100);
        }
        else if (vinkel > tilverdi)
        {
            sjekk2 = 1;
            motors.setSpeeds(-100, 100);
        }
        if (sjekk1 && sjekk2)
        {
            motors.setSpeeds(0, 0);
            break;
        }
    }
}

void whatPattern()
{ //Meny for hvilke mønster. 
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
void calibratepattern()//Spør om å kalibrere bilens gyroskop slik at den kan kjøre mønstrene ved hjelp av gyroen. Her bruker jeg Turnsensor.h
                       //til å kalibrere sensoren. Her har vi gjort noen endringer i "turnsensor.h" slik at vi kan kalibrere vilkårlig istedenfor kun i starten
{
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

void Patternmenu()//Funksjon som spilles av inne i ene meny casen. Det er en en switch case med de forskjellige tilfellene innenfor mønsterkjøring.
                  //Calibratepattern spør om å kalibrere, whatpattern spør om firkant, sirkel eller framtilbake og når disse er ferdige settes case variabelen
                  //til å være 0 samt meny hovedvariabelen.
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

void squarePattern()//Denne går i en firkant "duh", ved at den først venter i 200ms for så å kjøre i 800ms, snu, oppdatere vinkel for så å gjøre igjen helt
                    //til 4 rotasjoner er gjort.
{
    if (millis() - patternmillis > 200)
    {
        if (millis() - patternmillis < 1000 && turns < 4)
        {
            motors.setSpeeds(200, 200);
        }
        else if (turns == 4)
        {
            patternVar = 0;
            menuVar = 0;
        }
        else
        {
            turndeg(87);
            patternmillis = millis() - 200;
            turns++;
        }
    }
}
void circlePattern()//Denne utfører en sirkel ved at den kjører med 200,100 i fart helt til vinkelverdien er det den startet på/aka full sirkel.
{
    if (millis() - patternmillis > 500)
    {
        motors.setSpeeds(200, 100);
        turnSensorUpdate();
        vinkel = ((((int32_t)turnAngle >> 16) * -360) >> 16) + 180;
        if (vinkel == 175)
        {
            motors.setSpeeds(0, 0);
            patternVar = 0;
            menuVar = 0;
        }
    }
}
void forwardBackpattern()//Her kjører bilen fram og tilbake og gjør det på samme måte som firkanten, ved unntak at det er to rotasjoner og 180 grader snudd.
{
    if (millis() - patternmillis > 500)
    {
        motors.setSpeeds(200, 200);
        if (millis() - patternmillis > 2000)
        {
            turndeg(175);
            patternmillis = millis() - 500;
            turns++;
        }
        if (turns == 2)
        {
            patternVar = 0;
            menuVar = 0;
        }
    }
}

/////////////////////////PROX STOP/////////////////////////////////////
void footInFront()
{ //Registerer om noe er veldig nærme zumoen, og stopper den. 
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
{ /* Vil du tilbake til meny? Kommer fra footinfront
    Har også hidden feature*/
    if (proxClear == false)
    {
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
        secretCount = 0;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 2;
        proxClear = false;
        secretCount = 0;
    }
    if (buttonC.getSingleDebouncedPress())
    {
        secretCount += 1;
    }
    if (secretCount == 5)
    {
        menuVar = 9;
        display.clear();
    }
}

void secretMenu()
{ // Hemmelig meny
    if (buttonA.getSingleDebouncedPress())
    {
        hiddenFeature = true;
        menuVar = 0;
        secretCount = 0;
        display.clear();
    }

    if (buttonB.getSingleDebouncedPress())
    {

        if (superHiddenFeature == true) //Hvis superHiddenfeature er blitt brukt før
        {
            display.clear();
            display.gotoXY(3, 3);
            display.print("You can only use");
            display.gotoXY(1, 4);
            display.print("super charging once!");

            delay(2000);

            menuVar = 0;
            display.clear();
        }
        else
        {
            hiddenFeature = true;
            superHiddenFeature = true; //Første gangen superhiddenfeature blir brukt.
            menuVar = 10;
            display.clear();
        }
        secretCount = 0;
    }
}

void displaySecretMenu()
{ //Displayer hemmelig meny
    display.gotoXY(4, 0);
    display.print("Welcome to the");
    display.gotoXY(4, 1);
    display.print("hidden feature:");
    display.gotoXY(3, 2);
    display.print("Reverse charging!");
    display.gotoXY(5, 4);
    display.print("A");
    display.gotoXY(2, 6);
    display.print("Regular");
    display.gotoXY(2, 7);
    display.print("Charging");
    display.gotoXY(15, 4);
    display.print("B");
    display.gotoXY(13, 6);
    display.print("Super");
    display.gotoXY(13, 7);
    display.print("Charging");
}
//////////////////////////Software batteri/////////////////////////////
void batteryStatusTimer()
{ //Teller 10 sekund før den bytter til en annen variabel, slik at batterydisplay vises.
    batteryStatusMillis = millis();
    if (batteryStatusMillis - batteryStatusPreviousMillis > intervalBatteryStatus)
    {
        display.clear();
        batteryStatusDisplayVar = 1;
        batteryCaseBlock = true;
        batteryStatusPreviousMillis = batteryStatusMillis;
        batteryDisplayPreviousMillis = millis();
    }
}

void batteryDisplay()
{ //Vises i kort tid, og går så tilbake til normal display.
    batteryDisplayMillis = millis();
    display.gotoXY(0, 0);
    display.print(F("BatteryLevel: "));
    display.gotoXY(0, 1);
    display.print(batteryLife);
    display.gotoXY(0, 3);
    display.print(F("Charging cycle: "));
    display.gotoXY(0, 4);
    display.print(chargingCycle);
    display.gotoXY(0, 6);
    display.print(F("BatteryHealth: "));
    display.gotoXY(0, 7);
    display.print(batteryHealthPercentage);
    if (batteryDisplayMillis - batteryDisplayPreviousMillis > (intervalBatteryDisplay))
    {
        display.clear();
        batteryDisplayPreviousMillis = batteryDisplayMillis;
        batteryStatusDisplayVar = 0;
        batteryCaseBlock = false;
    }
}

void statusDisplay()
{ //Switch case som velger hva som skal displayes under kjøring.
    switch (batteryStatusDisplayVar)
    {
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
        break;
    case 3:
        chargeBatteryDisplay();
        break;
    case 4:
        emptyBatteryDisplay();
    case 5:
        batteryMalfunction();
    }
}

void emptyBatteryDisplay()
{ //Displayes hvis det er tomt battery
    motors.setSpeeds(0, 0);
    display.clear();
    menuVar = 3;
    chargeVar = 1;
}

void chargeBatteryDisplay()
{ //Stopper for at man må lade.
    if (fivePercentageStop == false)
    {
        display.clear();
        batteryHealthPercentage -= 5; // HVER GANG DEN GÅR TIL 5 PROSENT
        fivePercentageStop = true;    // SIDEN JEG IKKE HAR LADESTASJON SÅ SPØR DEN MED EN GANG
        updateBatteryHealthEeprom();
    }
    motors.setSpeeds(0, 0);

    menuVar = 3;
}

void lowBatteryDisplay()
{ //Hvis det er ti prosent batteri, sier ifra. 
    if (tenPercentClear == false)
    {
        display.clear();
        tenPercentClear = true;
    }

    display.gotoXY(5, 4);
    display.print(F("LOW BATTERY"));

    buzzer.playFrequency(800, 1000, 10);

    if ((batteryLife < 10))
    {
        batteryStatusDisplayVar = 0;
        display.clear();
    }
}

void batteryCase()
{ //Velger hva som skal displayes. 
    if (batteryCaseBlock == false)
    {
        if (batteryLife > 100)
        {
            batteryLife = 100;
        }
        else if (batteryLife > 10)
        {
            batteryStatusDisplayVar = 0;
        }
        else if (batteryLife > 5)
        {
            if (tenPercentClear == false)
            {
                batteryStatusDisplayVar = 2;
            }
        }
        else if (batteryLife > 0)
        {
            if (fivePercentageStop == false)
            {
                batteryStatusDisplayVar = 3; 
            }
        }
        else if (batteryLife == 0)
        {
            batteryStatusDisplayVar = 4;
        }
    }
}

// Batteri nivå fra 0-100
void batteryLevel()
{ //Tapper 1% hver gang man har kjørt over 20m
    if (speedDistance > 20)
    {
        batteryLife -= 1;
        speedDistance = 0;
    }
    if (hiddenFeature == true) //Hvis hidden feature er på, revers blir + på batteri. 
    {
        if (negativeTotalSpeed < 0)
        {
            if (speedDistance > 20)
            { // Absoultt verdi, derfor over 20
                batteryLife += 1;
                speedDistance = 0;
            }
        }
    }
}

void batteryLevelSuperHidden()
{ //Gjør det veldig raskt å rygge hvis man tar superhiddenfeature. 
    if (speedDistance > 50)
    {
        batteryLife += 10;
        speedDistance = 0;
    }
}

void batteryLevelDisplay()
{ //Batteriprosent displayes oppe til høyre. 
    if (batteryLife == 100)
    {
        display.gotoXY(18, 0);
        display.print(batteryLife);
    }
    else
    {
        display.gotoXY(19, 0);
        display.print(batteryLife);
    }
    if (batteryLife < 100 && batteryFull == 1)
    {
        display.clear();
        batteryFull = 0;
    }
}

///////////////////////Helsetilstand////////////////////////////////////
// Batteri helse fra 0-100, der 100 er beste verdi
void batteryHealth()
{ //Hvis batterihelsen blir 0 av en grunn, blir 0 istedet. 
    if (batteryHealthPercentage < 0)
    {
        batteryHealthPercentage = 0;
    }
}

void updateBatteryHealthEeprom()
{ //Oppdaterer eeprom til batterihelse
    EEPROM.write(batteryHealthAddress, batteryHealthPercentage);
}

void updateBankAccountEeprom()
{ //oppdaterer eeprom til bankkonto
    EEPROM.write(bankAccountAddress, bankAccount);
}

void chanceForReductionOfBatteryHealth()
{ //Random sjanse for at batterihelsen halveres. 
    long RandomCheckBatteryMalfunction = random(1, chanceForBatteryMalfunction);
    if (RandomCheckBatteryMalfunction == 1)
    {
        batteryHealthPercentage = batteryHealthPercentage / 2;
        updateBatteryHealthEeprom();
        display.clear();
        batteryStatusDisplayVar = 5;
        batteryHealthPreviousMillis = batteryHealthMillis;
    }
}

void batteryMalfunction()
{ //Display for hvis batterihelsen halveres
    batteryHealthMillis = millis();
    display.gotoXY(5, 4);
    display.print(F("MALFUNCTION"));
    display.gotoXY(5, 5);
    display.print(F("BATTERY HALVED"));

    buzzer.playNote(NOTE_A(4), 500, 15);

    if (batteryHealthMillis - batteryHealthPreviousMillis > (5 * oneSecond))
    {
        batteryStatusDisplayVar = 0;
        batteryHealthPreviousMillis = batteryHealthMillis;
        display.clear();
    }
}

void batteryHealthAfterAverageMeasure()
{ //Hvor mye gjennomsnittsverdiene skal gi effekt på batterihelsen. 
    int batteryEffectSecondsAboveSeventy = secondsAboveSeventy * 0.25;
    int batteryEffectAverageSpeed60Sec = averageSpeed60Sec * 0.25;
    int batteryEffectMaxSpeed = maxSpeed * 0.1;
    int batteryEffectSum = (batteryEffectAverageSpeed60Sec + batteryEffectSecondsAboveSeventy) * batteryEffectMaxSpeed;
    batteryHealthPercentage -= batteryEffectSum;
    Serial.println(batteryEffectSum);
    Serial.print(batteryHealthPercentage);
    updateBatteryHealthEeprom();
    buzzer.playFrequency(250,1000,15);
}


void chargingMenu()
{ //LadeMeny
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
        batteryChargingMenu();
        break;
    case 3:
        batteryServiceOrChangeMenu();
        break;
    }
}

void notEnoughMoney()
{ //Tar deg tilbake til start siden man ikke har nok penger. 
    display.clear();
    display.gotoXY(0,3);
    display.print("Not enough money!");
    display.gotoXY(0,4);
    display.print("Transfering to menu");
    delay(3000);
    menuVar = 0;
}

void batteryChargingMenu()
{ //Hvor mye strøm vil man kjøpe? 10%, 25% eller fulladet.
    display.gotoXY(0, 0);
    display.print("How much do you want?");
    display.gotoXY(0, 2);
    display.print("A = full battery->");
    display.gotoXY(18, 2);
    display.print(batteryCost);
    display.gotoXY(0, 4);
    display.print("B for +25% -> 50$");
    display.gotoXY(0, 6);
    display.print("C for +10% -> 20$");

    if (buttonA.getSingleDebouncedPress())
    {
       if (bankAccount < batteryCost){
        notEnoughMoney();
       } else{
        batteryHealthPercentage -= 3;
        updateBatteryHealthEeprom();
        chargeVar = 1;
        chargingCycle += 1;
        bankAccount -= batteryCost;
        updateBankAccountEeprom();
        batteryLife = 100;
        batteryFull = 1;
        display.clear();
       }
    }
    if (buttonB.getSingleDebouncedPress())
    {
        if (bankAccount < 50){
            notEnoughMoney();
        } else{
        batteryHealthPercentage -= 3;
        updateBatteryHealthEeprom();
        batteryLife += 25;
        chargeVar = 1;
        chargingCycle += 1;
        bankAccount -= 50;
        updateBankAccountEeprom();
        if (batteryLife >= 100)
        {
            batteryLife = 100;
            batteryFull = 1;
        }
        display.clear();
        }
    }
    if (buttonC.getSingleDebouncedPress())
    {
        if (bankAccount < 20){
            notEnoughMoney();
        } else{
        batteryHealthPercentage = batteryHealthPercentage - 3;
        updateBatteryHealthEeprom();
        chargeVar = 1;
        batteryLife += 10;
        chargingCycle += 1;
        bankAccount -= 20;
        updateBankAccountEeprom();
        if (batteryLife >= 100)
        {
            batteryLife = 100;
            batteryFull = 1;
        }
        display.clear();
        }
    }
}

void doYouWantToCharge()
{ //Vil du lagre? Viser også info om batteri og konto. 
    aAndBFor();
    if (batteryHealthPercentage > 99){
        batteryHealthPercentage = 99;
    }
    display.gotoXY(0, 0);
    display.print(F("Do you want to charge?"));
    display.gotoXY(0, 5);
    display.print("Bank account: ");
    display.gotoXY(18, 5);
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
        if (batteryStatusDisplayVar == 3)
        {
            menuVar = 2;
            batteryStatusDisplayVar = 0;
            display.clear();
        }
        else
        {
            menuVar = 0;
            display.clear();
        }
    }
}

void chargingOrBatteryService()
{ //Vil du lade eller fikse batterihelse?
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
        batteryCost = (100 - batteryLife) * 2;
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

void batteryHealthCritical()
{ //Display på at batterihelsen er dø. 
    display.gotoXY(0, 2);
    display.print("BATTERY CRITICAL!");
    display.gotoXY(3, 3);
    display.print("FIX NOW!");
}

void batteryHealthLevel0Or1()
{   /*Hvis batterihelsen går ned til 10. Første gang vil den gå til service, 
    men andre gangen må man bytte batteri
    */
    if (batteryHealthPercentage <= 10 && batteryHealthLevel == 0)
    {
        buzzer.playFrequency(1000, 1000, 15);
        display.clear();
        serviceVar = 1;
        menuVar = 6;
        batteryHealthLevel = 1;
        delay(3000);
    }
    if (batteryHealthPercentage <= 10 && batteryHealthLevel == 1)
    {
        buzzer.playFrequency(1000, 1000, 15);
        display.clear();
        serviceVar = 2;
        menuVar = 6;
        batteryHealthLevel = 0;
        delay(3000);
    }
}

void batteryServiceOrChangeMenu()
{   //Vil du bytte batteri eller ta service på batterihelsen?
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

void batteryHealthServiceCost()
{ //Hvor mye service koster
    if (batteryHealthPercentage <= 10)
    {
        display.gotoXY(0, 0);
        display.print("BatteryHealth too low");
        display.gotoXY(0, 1);
        display.print("must change battery");
        display.gotoXY(0, 3);
        display.print("transfering to menu");
        delay(1000);
        menuVar = 0;
    }
    if (batteryHealthPercentage >= 10)
    {

        batteryCost = (100 - batteryHealthPercentage) * 2;
        updatedBatteryHealthPercentage = batteryHealthPercentage + 20;
    }
    if (updatedBatteryHealthPercentage > 99)
    {
        updatedBatteryHealthPercentage = 99;
    }
    serviceWillCostMenu();

    if (buttonA.getSingleDebouncedPress())
    {
        if (bankAccount < batteryCost){
            notEnoughMoney();
        } else{
        calibratePaymentBatteryService();
        updateBankAccountEeprom();
        }
    }

    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        menuVar = 0;
    }
}

void serviceWillCostMenu()
{ //display på hvor mye batteriet koster. 
    display.gotoXY(0, 0);
    display.print("Service will cost:");
    display.gotoXY(18, 0);
    display.print(batteryCost);
    display.gotoXY(0, 1);
    display.print("Updated health:");
    display.gotoXY(18, 1);
    display.print(updatedBatteryHealthPercentage);
    display.gotoXY(0, 4);
    display.print("Do you want to pay?");
    aAndBFor();
    display.gotoXY(0, 7);
    display.print("Bank account:");
    display.gotoXY(14, 7);
    display.print(bankAccount);
}

void batteryChange()
{ //Display på hvor mye endring av batteri vil koste. 
    display.gotoXY(0, 0);
    display.print("This will cost:");
    display.gotoXY(15, 0);
    display.print(batteryChangeCost);
    display.gotoXY(0, 1);
    display.print("Do you want to change?");
    display.gotoXY(0, 3);
    aAndBFor();
    display.gotoXY(0, 7);
    display.print("Bank account:");
    display.gotoXY(14, 7);
    display.print(bankAccount);

    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        if (bankAccount < batteryChangeCost)
        {
            notEnoughMoney();
        }
        else{
            bankAccount -= batteryChangeCost;
            batteryHealthPercentage = 99;
            batteryHealthLevel = 0;
            updateBatteryHealthEeprom();
            updateBankAccountEeprom();
            menuVar = 0;
        }
    }
    if (buttonB.getSingleDebouncedPress()){
        menuVar = 0;
    }
}

void calibratePaymentBatteryService()
{ //Kalibrerer kjøp og går til meny. 
    display.clear();
    display.gotoXY(0, 3);
    display.print("Calibrating payment...");
    bankAccount -= batteryCost;
    updateBankAccountEeprom();
    batteryHealthPercentage += 20; // fikser bare 20%
    delay(3000);
    display.clear();
    display.gotoXY(0, 3);
    display.print("Payment done!");
    display.gotoXY(0, 5);
    display.print("Returning to menu...");
    delay(3000);
    display.clear();
    menuVar = 0;
}

void batteryService()
{  //Batteriservice switch case
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

void hiddenFeatureTimer()
{ //Så lenge hiddenFeatureTimer
    if (hiddenFeature == true)
    {
        unsigned long hiddenFeatureMillis = millis();
        if (hiddenFeatureMillis - prevHiddenFeatureMillis == hiddenFeatureInterval)
        {
            hiddenFeature = false;
        }
    }
}

//////////////////MENU////////////////////////
void menu()
{ //Menyswitchcase
    hiddenFeatureTimer();
    if (batteryHealthPercentage > 99){
        batteryHealthPercentage = 99;
    }
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
        patternVar = 0;
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
        display.gotoXY(0, 0);
        display.print("Download other file");
        break;
    case 9:
        displaySecretMenu();
        secretMenu();
        break;
    case 10:
        reversingHiddenFeature();
        batteryLevelSuperHidden();
        batteryLevelDisplay();
        speedometer();
        break;
    }
}

void reversingHiddenFeature()
{ //Rygger og lader frem til 20%
    motors.setSpeeds(-200, -200);
    display.gotoXY(0, 3);
    display.print("Charging....");
    if (batteryLife >= 20)
    {
        if (batteryLife < 20){
            batteryLife = 20;
        }
        menuVar = 0;
        motors.setSpeeds(0, 0);
    }
}

void pressFor()
{ //Display for hver gang man ønsker dette skrevet:
    display.gotoXY(0, 0);
    display.print(F("Press A for: "));
    display.gotoXY(0, 3);
    display.print(F("Press B for: "));
    display.gotoXY(0, 6);
    display.print(F("Press C for: "));
}

void aAndBFor()
{ //display for hver gang man ønsker dette skrevet:
    display.gotoXY(0, 3);
    display.print(F("A for YES"));
    display.gotoXY(12, 3);
    display.print(F("B for NO"));
}

void menuDisplay1()
{ //Første siden på menydisplay
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

void menuDisplay2()
{ //2. siden på menydisplay
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
    menu(); //Switch case meny.
}
