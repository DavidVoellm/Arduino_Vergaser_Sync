#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
#define knopfPin 13
#define pressurePin1 A0
#define pressurePin2 A1
int bias = 0;
bool gedrueckt = false;
String content[] = {"",""};
int last_pressure1 = 0;
int last_pressure2 = 0;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(pressurePin1, INPUT);
  pinMode(pressurePin2, INPUT);
  Serial.begin(9600);
}
void setDisplay(int displayID, String text){
  if ( content[displayID] == text ) return;
  content[displayID] = text;
  lcd.setCursor(0,displayID);
  lcd.print(text+"                       ");
}
void setDisplay(int displayID, float text)
  {return setDisplay(displayID, (String)text);}

void setDisplay(int displayID, int text)
  {return setDisplay(displayID, (String)text);}

int druckInmBar(int pin){ // Pbar = (10*Vout/Vs +0.95)/9
  float pressure = analogRead(pin);
  return (int)((10*pressure/1024.0+0.95)/.009);
}

void durchlauf(){
  int pressure1 = druckInmBar(pressurePin1);
  if (abs(last_pressure1-pressure1)<=1){
    pressure1 = last_pressure1;
  }
  int pressure2 = druckInmBar(pressurePin2);
  if (abs(last_pressure2-pressure2)<=1){
    pressure2 = last_pressure2;
  }
  if (pressure1 != last_pressure1 || pressure2 != last_pressure2){
    last_pressure1 = pressure1;
    last_pressure2 = pressure2;
    int difference = pressure2 - pressure1 + bias;
    updateDisplay(pressure1, pressure2);
  }
}
void updateDisplay(int pressure1, int pressure2){
  int difference = pressure2 - pressure1 + bias;
  Serial.print("Diff:"+(String)difference);
  Serial.print(",P1:"+(String)pressure1);
  Serial.print(",P2:"+(String)pressure2);
  Serial.println(",bias:"+(String)(-bias));
  setDisplay(0,"Diff.:"+(String)difference);
  setDisplay(1,"P1:"+(String)pressure1+"|P2:"+(String)pressure2);
}
void setBias(int pressure1, int pressure2){
  bias = pressure1 - pressure2;
  Serial.println("Set BIAS: "+ (String)bias);
}
void resetBias(){
  bias = 0;
}

void loop() {
  delay(1);
  durchlauf();
  gedrueckt = digitalRead(knopfPin);
  if (gedrueckt){
    int startTime = millis();
    while(gedrueckt){gedrueckt = digitalRead(knopfPin);}
    int pressure1 = druckInmBar(pressurePin1);
    int pressure2 = druckInmBar(pressurePin2);
    int now = millis();
    if (now-startTime<800){
      setBias(pressure1, pressure2);
    }
    else{
      //Serial.println("TIME: "+ (String)(now-startTime) + "  NOW: " +(String)(now)+ "  Start: "+(String)(startTime));
      resetBias();
    }
    Serial.println(bias);
    
    updateDisplay(pressure1, pressure2);
  }
}
