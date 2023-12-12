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

int bankAccountAddress = 1;
int bankAccount = EEPROM.read(bankAccountAddress);

unsigned long intervalCustomer = random(10000,20000);
unsigned long iceCreamMillis;
unsigned long previousIceCreamMillis = 0;

int jobVar = 0;
int songVar = 0;
int bankEarnings = 0;
int amountOfSongs = 3;
bool proxClear = false;

unsigned int lineSensorValues[5] = {0, 0, 0, 0, 0}; // 5 sensorer
const uint16_t followLinemaxSpeed = 300;
int16_t lastError = 0;

//Trenger ikke å tenke på batterihelse eller batteriliv. Han får gratis av jobben under jobb.

const char march[] PROGMEM = 
"! O2 T100 MS"
"a8. r16 a8. r16 a8. r16 f8 r16 >c16" 
"ML"
"a8. r16 f8 r16" "MS" ">c16 a. r8"
"O3"
"e8. r16 e8. r16 e8. r16 f8 r16 <c16"
"O2 ML"
"a-8. r16" "MS" "f8 r16 >c16 a r"

"O3 ML"
"a8. r16 <a8 r16 <a16" "MS" "a8. r16 a-8 r16 g16"
"ML V10"
"g-16 f16 g-16 r16 r8 <b-16 r16" "MS" "e-8. r16 d8." "ML" "d-16"
"c16 <c-16 c16 r16 r8" "MS O2" "f16 r16 a-8. r16 f8. a-16"
"O3"
"c8. r16 <a8 r16 c16 e2. r8"

"O3 ML"
"a8. r16 <a8 r16 <a16" "MS" "a8. r16 a-8 r16 g16"
"ML V10"
"g-16 f16 g-16 r16 r8 <b-16 r16" "MS" "e-8. r16 d8." "ML" "d-16"
"c16 <c-16 c16 r16 r8" "MS O2" "f16 r16 a-8. r16 f8. >c16"
"ML"
"a8. r16 f8 r16 >c16 a2. r8"
;

const char fugue[] PROGMEM =
  "! O5 L16 agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6 dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8 MS <b-d<b-d MLe-<ge-<g MSc<ac<a ML d<fd<f O5 MS b-gb-g"
  "ML >c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6 L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5 e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe f O6 dc#dfdc#<b c#4";


void setup(){
    Serial.begin(9600);
    lineSensors.initFiveSensors();
    display.clear();
    display.setLayout21x8();
    randomSeed(0);
}

void whatJobDoYOuWant(){
    display.gotoXY(0,0);
    display.print("What job do you want?");
    display.gotoXY(0,2);
    display.print("Bank account:");
    display.gotoXY(13,2);
    display.print(bankAccount);
    display.gotoXY(5,6);
    display.print("A");
    display.gotoXY(14,6);
    display.print("B");
    display.gotoXY(4,7);
    display.print("Sing");
    display.gotoXY(10,7);
    display.print("Ice cream");

    if (buttonA.getSingleDebouncedPress()){
        jobVar = 1;
        display.clear();
    }
    if (buttonB.getSingleDebouncedPress()){
        jobVar = 3;
        display.clear();
    }

}

void songOne(){
    display.gotoXY(0,0);
    display.print("Star wars playing...");
    
    if (buzzer.isPlaying())
    {  
    }
    else
    {
      buzzer.playFromProgramSpace(march);
    }
    delay(5000);
    songVar = 0;
    jobVar = 0;
    display.clear();
    bankEarnings = random(50);
    display.clear();
    display.gotoXY(0,0);
    display.print("WOW! You earned:");
    display.gotoXY(0,1);
    display.print(bankEarnings);
    display.gotoXY(0,5);
    display.print("Returning to menu");
    bankAccount += bankEarnings;
    if (bankAccount > 255){
    bankAccount = 255;
    }
    EEPROM.write(bankAccountAddress, bankAccount);
    delay(20000);
    display.clear();
}

void songTwo(){
    display.gotoXY(0,0);
    display.print("Playing song two");
    
    buzzer.playFrequency(440, 200, 15);

  // Delay to give the tone time to finish.
  delay(1000);

  // Start playing note A in octave 4 at maximum volume
  // volume (15) for 2000 milliseconds.
  buzzer.playNote(NOTE_A(4), 2000, 15);

  // Wait for 200 ms and stop playing note.
  delay(200);
  buzzer.stopPlaying();

  delay(1000);

  // Start playing a fugue from program space.
  buzzer.playFromProgramSpace(fugue);

  // Wait until it is done playing.
  while(buzzer.isPlaying()){ }

  delay(1000);
  songVar = 0;
  jobVar = 0;
  bankEarnings = random(50);
  display.clear();
  display.gotoXY(0,0);
  display.print("WOW! You earned:");
  display.gotoXY(0,1);
  display.print(bankEarnings);
  display.gotoXY(0,5);
  display.print("Returning to menu");
  bankAccount += bankEarnings;
  if (bankAccount > 255){
    bankAccount = 255;
  }
  EEPROM.write(bankAccountAddress, bankAccount);
  delay(5000);
  display.clear();
}

void sing(){
    switch (songVar){
        case 0:
            songVar = random(amountOfSongs);
            display.clear();
            break;
        case 1:
            songOne();
            break;
        case 2:
            songTwo();
            break;
    }
 }


void iceCreamDelivery(){
    driveLinePID();
    iceCreamMillis = millis();
    if (iceCreamMillis - previousIceCreamMillis > intervalCustomer){
        youHaveACustomer();
        previousIceCreamMillis = iceCreamMillis;
    }
    display.gotoXY(0,6);
    display.print("Bank account:");
    display.gotoXY(13,6);
    display.print(bankAccount);
}

void youHaveACustomer(){
    buzzer.playFrequency(1000,1000,10);
    motors.setSpeeds(0,0);
    bankEarnings = random(50);
    display.gotoXY(0,0);
    display.print("You have a customer!");
    display.gotoXY(0,2);
    display.print("You earned:");
    display.gotoXY(11,2);
    display.print(bankEarnings);
    bankAccount += bankEarnings;
    delay(5000);
    display.clear();
}

void footInFront()
{
    proxSensor.read();
    uint16_t frontleftPosition = proxSensor.countsFrontWithLeftLeds();
    uint16_t frontrightPosition = proxSensor.countsFrontWithRightLeds();

    if ((frontleftPosition == 6) || (frontrightPosition == 6))
    {
        motors.setSpeeds(0, 0);
        display.clear();
        jobVar = 4;
    }
}

void driveLinePID()
{
    int16_t position = lineSensors.readLine(lineSensorValues);

    int16_t error = position - 2000;
    int16_t speedDifference = error / 0.5 + 4 * (error - lastError);

    lastError = error;

    int leftSpeed = followLinemaxSpeed + speedDifference;
    int rightSpeed = followLinemaxSpeed - speedDifference;
    leftSpeed = constrain(leftSpeed, 0, (int16_t)followLinemaxSpeed);
    rightSpeed = constrain(rightSpeed, 0, (int16_t)followLinemaxSpeed);

    motors.setSpeeds(leftSpeed, rightSpeed);
}

void doYouWantToCalibrate()
{
    display.gotoXY(0, 0);
    display.print(F("Want to calibrate?"));
    display.gotoXY(0, 3);
    display.print(F("A for YES"));
    display.gotoXY(12, 3);
    display.print(F("B for NO"));

    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        calibrating();
        jobVar = 2;
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        jobVar = 2;
    }
}

void backToMenu()
{
    if (proxClear == false)
    {
        display.clear();
        proxClear = true;
    }
    display.gotoXY(0, 0);
    display.print(F("Back to menu?"));
    display.gotoXY(0, 3);
    display.print(F("A for YES"));
    display.gotoXY(12, 3);
    display.print(F("B for NO"));
    if (buttonA.getSingleDebouncedPress())
    {
        display.clear();
        jobVar = 0;
        proxClear = false;
        
    }
    if (buttonB.getSingleDebouncedPress())
    {
        display.clear();
        jobVar = 2;
        proxClear = false;
        
    }
}

void jobMenu(){
    switch (jobVar){
        case 0:
            whatJobDoYOuWant();
            break;
        case 1: 
            sing();
            break;
        case 2:
            iceCreamDelivery();
            footInFront();
            break;
        case 3:
            doYouWantToCalibrate();
            break;
        case 4:
            backToMenu();
            break;
    }
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



void loop(){
    jobMenu();
}