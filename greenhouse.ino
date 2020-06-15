#include "Nokia_5110.h"
#include<EEPROM.h>

#define RST 3
#define CE 4
#define DC 5
#define DIN 6
#define CLK 7

Nokia_5110 lcd = Nokia_5110(RST, CE, DC, DIN, CLK);


#include <DHT.h>

#define DHTPIN 8
#define DHTTYPE DHT11
#define modeBTN 2
#define incBTN A3
#define decBTN A2

DHT dht(DHTPIN, DHTTYPE);

#define pump 11
#define fan1 12
#define humidifier 9
#define lamp 10
#define soil A0
#define ldr A1

unsigned long lastclear = 0;
unsigned long lastdht = 0;
unsigned long lastbtn = 0;
byte mode = 0;
byte temp;
byte humidity;
byte light;
byte soilset;
byte last_temp;
byte last_hum;
byte last_light;
byte last_soil;
bool clearflag=true;

void modeSelect(){
  Serial.println("Pressed mode button");
  while(!digitalRead(modeBTN));
  lastbtn = millis();
  mode++;
  if(mode>=5) mode=1;
  lcd.clear();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("Greenhouse"));
  dht.begin(); 
  lcd.setCursor(10,2);
  lcd.print("GREENHOUSE");
  delay(2000);
  lcd.clear();

  pinMode(pump, OUTPUT);
  pinMode(fan1, OUTPUT);
  pinMode(humidifier, OUTPUT);
  pinMode(lamp, OUTPUT);
  pinMode(modeBTN,INPUT_PULLUP);
  pinMode(incBTN, INPUT_PULLUP);
  pinMode(decBTN, INPUT_PULLUP);
  digitalWrite(pump, LOW);
  digitalWrite(fan1, LOW);
  digitalWrite(humidifier, HIGH);
  digitalWrite(lamp, HIGH);

  temp=EEPROM.read(0);
  humidity=EEPROM.read(1);
  light=EEPROM.read(2);
  soilset=EEPROM.read(3);
  last_temp=temp;
  last_hum=humidity;
  last_light=light;
  last_soil=soilset;
  Serial.print("Initial Temperature threshold: ");
  Serial.println(temp);
  Serial.print("Initial Humidity threshold: ");
  Serial.println(humidity);
  Serial.print("Initial Light mode: ");
  Serial.println(light);
  Serial.print("Initial Soil moisture threshold: ");
  Serial.println(soilset);

  attachInterrupt(digitalPinToInterrupt(modeBTN), modeSelect, RISING);
  mode=0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis()<2000) mode=0;
  
  while(mode){
    //Serial.println("Entering mode screen");
    //display mode screen
    lcd.setCursor(0,0);
    lcd.print("   Settings   ");
    
    switch(mode){
      case 1: //Set temperature
              if(millis()- lastclear > 500){
                Serial.println("Entered into Temp Mode");
                lastclear = millis();
                lcd.setCursor(0,1);
                clearflag = !clearflag;
                if(clearflag)  lcd.print("Temp: ");
                else           lcd.print("                ");
                lcd.setCursor(50,1);
                lcd.print(temp);
                lcd.print('C');
                print_hum();
                print_light();
                print_soil();
              }
              if(!digitalRead(incBTN))
               {
                while(!digitalRead(incBTN));
                lastbtn = millis();
                temp++;
               }
              else if(!digitalRead(decBTN))
               {
                while(!digitalRead(decBTN));
                lastbtn = millis();
                temp--;
               }  
              break;
      case 2: //Set humidity
              if(millis()- lastclear > 500){
                Serial.println("Entered into Humidity Mode");
                lastclear = millis();
                lcd.setCursor(0,2);
                clearflag = !clearflag;
                if(clearflag)  lcd.print("Humidity: ");
                else           lcd.print("                ");
                lcd.setCursor(50,2);
                lcd.print(humidity);
                lcd.print('%');
                print_temp();
                print_light();
                print_soil();
              }
              if(!digitalRead(incBTN))
               {
                while(!digitalRead(incBTN));
                lastbtn = millis();
                humidity++;
               }
              else if(!digitalRead(decBTN))
               {
                while(!digitalRead(decBTN));
                lastbtn = millis();
                humidity--;
               }
              break;
      case 3: //Set light
              if(millis()- lastclear > 500){
                Serial.println("Entered into Light Mode");
                lastclear = millis();
                lcd.setCursor(0,3);
                clearflag = !clearflag;
                if(clearflag)  lcd.print("Light: ");
                else           lcd.print("                ");
                lcd.setCursor(50,3);
                if(light==1)      lcd.print("OFF  ");
                else if(light==2) lcd.print("AUTO ");
                else if(light==3) lcd.print("ON   ");
                print_temp();
                print_hum();
                print_soil();
              }
              if(!digitalRead(incBTN))
               {
                while(!digitalRead(incBTN));
                lastbtn = millis();
                light++;
                if(light>=3) light=3;
               }
              else if(!digitalRead(decBTN))
               {
                while(!digitalRead(decBTN));
                lastbtn = millis();
                light--;
                if(light<=1) light=1;
               }
              break;
      case 4: //Set soil moisture
              if(millis()- lastclear > 500){
                Serial.println("Entered into Soil Mode");
                lastclear = millis();
                lcd.setCursor(0,4);
                clearflag = !clearflag;
                if(clearflag)  lcd.print("Soil: ");
                else           lcd.print("                ");
                lcd.setCursor(50,4);
                lcd.print(soilset);
                print_temp();
                print_hum();
                print_light();
              }
              if(!digitalRead(incBTN))
               {
                while(!digitalRead(incBTN));
                lastbtn = millis();
                soilset+=5;
               }
              else if(!digitalRead(decBTN))
               {
                while(!digitalRead(decBTN));
                lastbtn = millis();
                soilset-=5;
               }
              break;
    }
    //while(millis() - lastbtn < 200);
  
    if(millis()-lastbtn > 5000){
      mode=0;
      if(last_temp!=temp){
        EEPROM.write(0,temp);
        Serial.print("Set Temperature: ");
        Serial.println(temp);
        last_temp=temp;
      }
      if(last_hum!=humidity){
        EEPROM.write(1,humidity);
        Serial.print("Set Humidity: ");
        Serial.println(humidity);
        last_hum=humidity;
      }
      if(last_light!=light){
        EEPROM.write(2,light);
        Serial.print("Set Lamp mode: ");
        Serial.println(light);
        last_light=light;
      }
      if(last_soil!=soilset){
        EEPROM.write(3,soilset);
        Serial.print("Set Soil Moisture : ");
        Serial.println(soilset);
        last_soil=soilset;
      }
      Serial.println("Exiting mode screen");
      lcd.clear();            
    }
  }

  
  if(millis() - lastdht > 2000){
    //lcd.clear();
    
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    lastdht = millis();

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.println();

    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print("C");
    lcd.setCursor(0,1);
    lcd.print("Humidity: ");
    lcd.print((int)h);
    lcd.print("%");

    lcd.setCursor(0,2);
    if(t > temp){
      digitalWrite(fan1, HIGH);      
      Serial.println("Fan is on");
      lcd.print("Fan is on     ");
    }
    else{
      digitalWrite(fan1, LOW);
      Serial.println("Fan is off");
      lcd.print("Fan is off     ");
    }

    lcd.setCursor(0,5);
    if((int)h > humidity){
      digitalWrite(humidifier, HIGH);
      Serial.println("Humidifier is off");
      lcd.print("Humifier is off");
    }
    else{
      digitalWrite(humidifier, LOW);
      Serial.println("Humidifier is on");
      lcd.print("Humifier is on  ");
    }
    
      
    byte soilmois = map(analogRead(soil), 0, 1023, 0, 255);
    Serial.print("Soil moisture value : ");
    Serial.println(soilmois);

    lcd.setCursor(0,3);
    if(soilmois > soilset){
      digitalWrite(pump, HIGH);
      Serial.println("Pump is on");
      lcd.print("Pump is on     ");
    }
    else{
      digitalWrite(pump, LOW);
      Serial.println("Pump is off");
      lcd.print("Pump is off     ");
    }

    lcd.setCursor(0,4);
    byte night = digitalRead(A1);
    if((night && light==2) || light==3){
      digitalWrite(lamp, LOW);
      Serial.println("Light is on");
      lcd.print("Light is on     ");
    }
    else if((!night && light==2) || light==1){
      digitalWrite(lamp, HIGH);
      Serial.println("Light is off");
      lcd.print("Light is off     ");
    }

  }
}

void print_temp(){
    lcd.setCursor(0,1);
    lcd.print("Temp: ");
    lcd.setCursor(50,1);
    lcd.print(temp);
    lcd.print("C");
}
void print_hum(){
    lcd.setCursor(0,2);
    lcd.print("Humidity: ");
    lcd.setCursor(50,2);
    lcd.print(humidity);
    lcd.print("%");
}
void print_light(){
    lcd.setCursor(0,3);
    lcd.print("Light: ");
    lcd.setCursor(50,3);
    if(light==1)      lcd.print("OFF  ");
    else if(light==2) lcd.print("AUTO ");
    else if(light==3) lcd.print("ON   ");
}
void print_soil(){
    lcd.setCursor(0,4);
    lcd.print("Soil: ");
    lcd.setCursor(50,4);
    lcd.print(soilset);
}
