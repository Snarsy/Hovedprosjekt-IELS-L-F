int redLED = 10;     
int greenLED = 9;    
int buzzerPin = 11;

int LED1 = 6;
int LED2 = 7;
int SW1 = 3;	
int SW2 = 4;	

int winnerBeep = 750;       
int faultBeep = 200;    

unsigned long wait = 0;
unsigned long now = 0;

int vinner = 0;
int taper = 0;

int placetime = 0;

void reset(){
    digitalWrite(greenLED,LOW);
    digitalWrite(redLED,HIGH);
    
    vinner = 0;
    taper = 0;
    placetime=1;
}
void pause(){
    now = millis();
    wait = now+random(3000,7000);
    while (now<wait){
        if(digitalRead(SW1)==LOW)taper = LED1;
        if(digitalRead(SW2)==LOW)taper = LED2;
        if (taper!=0){
            placetime = 4;
        }
        else {
            placetime = 2;
        }
        now=millis();
    }
    
}
void kjor(){
    digitalWrite(redLED,LOW);
    digitalWrite(greenLED,HIGH);
    while (vinner==0){
        if(digitalRead(SW1)==LOW){
            vinner = LED1;
        }
        else if(digitalRead(SW2)==LOW){
            vinner = LED2;
        }
    }
    placetime = 3;
}
void vinn(){
    for(int k = 0; k < 5; k++){
      tone(buzzerPin,(winnerBeep+(k*20)));
      digitalWrite(greenLED,HIGH);
      digitalWrite(vinner,HIGH);
      delay(50);
      digitalWrite(vinner,LOW);
      digitalWrite(greenLED,LOW);
      delay(50); 
    }
    noTone(buzzerPin);
    placetime = 0;
}
void tap(){
    tone(buzzerPin,faultBeep,500);
    for(int k = 0; k < 10; k++){
      digitalWrite(redLED,HIGH);
      digitalWrite(taper,HIGH);
      delay(50);
      digitalWrite(redLED,LOW);
      digitalWrite(taper,LOW);
      delay(50); 
    }
    placetime = 0;
}

void setup(){
    pinMode(redLED,OUTPUT);
    pinMode(greenLED,OUTPUT);
    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(SW1,INPUT);
    pinMode(SW2,INPUT);
}
void loop(){
    switch (placetime){
        case 0:
            reset();
            break;
        case 1:
            pause();
            break;
        case 2:
            kjor();
            break;
        case 3:
            vinn();
            break;
        case 4:
            tap();
            break;
    }
}