//Variablene trengt til flexsensoren
int flexvalue;
int flexlow = 1000;
int flexhigh = 0;

//Variablene og funksjonen til avstandsmåleren
int avstandlow = 1000;
int avstandhigh = 0;
int tid = 0;
float avstand;
void avstandmaler(){
    digitalWrite(10, LOW);  
	delayMicroseconds(2);  
	digitalWrite(10, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(10, LOW);  
    tid = pulseIn(11, HIGH); 
    avstand = (tid*.0343)/2;   
}

//Variablene til lyssensoren
int lightlow = 1000;
int lighthigh = 0;
int lightvalue;
void setup(){
    Serial.begin(9600);

    pinMode(A0,INPUT_PULLUP);//Initialisering av flexsensoren
    pinMode(3,OUTPUT);

    pinMode(11,INPUT);//Initialisering av avstandsmåleren
    pinMode(10,OUTPUT);
    pinMode(9,OUTPUT);

    pinMode(A1,INPUT_PULLUP);//Initialisering av lyssensoren
    pinMode(6,OUTPUT);
}
void loop(){
    //Mapper verdiene til flexresistoren mellom 0 og 255 som så vises i lysdioden 3
    if (analogRead(A0)<flexlow){flexlow=analogRead(A0);}
    if (analogRead(A0)>flexhigh){flexhigh=analogRead(A0);}
    flexvalue=map(analogRead(A0),flexlow,flexhigh,0,255);
    analogWrite(3,flexvalue);
    Serial.println(flexvalue);

    //Mapper verdiene til avstandsmåleren mellom 0-255 som vises i lysdiode 9. Men her ville dessverre ikke bunn og toppverdier kalibreres riktig og måtte settes inn for hand
    avstandmaler();
    avstand=map(avstand,2,75,0,255);
    analogWrite(9,avstand);
    //Serial.println(avstand);

    //Mapper verdiene til lyssensoren mellom 0-255 for så å vise det på diode ved utgang 6
    if (lightlow>analogRead(A1)){lightlow=analogRead(A1);}
    if (lighthigh<analogRead(A1)){lighthigh=analogRead(A1);}
    lightvalue=map(analogRead(A1),lightlow,lighthigh,0,255);
    analogWrite(6,lightvalue);
    //Serial.println(lightvalue);
}