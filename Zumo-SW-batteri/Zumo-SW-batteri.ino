#include <Zumo32U4.h>


Zumo32U4OLED display;

int batterylife = 100;
int distance = 0;
int interval = 20;

void setup(){
    display.setLayout21x8();
    display.clear();
}

void loop(){
    kjor();
    speedometer();
    batterylevel();
    batteryleveldisplay();
}

void kjor(){//Bilen kjører, ved å enten følge linje eller mønster

}
void speedometer(){//Sjekker hvor langt bilen har kjørt og hvor fort den kjører
    //Sett distance
}
void batterylevel(){//Sjekker om avstand kjørt er lengre enn vist intervall. vil så redusere batteri
    if(distance>interval){
        batterylife--;

    }
}
void batteryleveldisplay(){//Viser batterinivå
    //Vis batterinivå
}