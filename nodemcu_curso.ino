#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "DHT.h"

#define ledBlue 4
#define ledGreen 0
#define ledRed 2
#define ldr A0
#define sw 15

#define HUMIDITY 1
#define LED_ON 2
#define LDR 3
#define TURN_ON 4
#define TEMPERATURE 5
#define DHTTYPE DHT11

bool ledRedOn = false;
bool ledGreenOn = false;
bool ledBlueOn = false;

int  ldrValue = 0;
int humidityValue = 0;
int temperatureValue = 0;
int turnOn = 0;
bool changeValues = true;

const char* ssid = "TIGO-B13F";
const char* password = "2NB112101391";
const char* claveApiWrite = "HWVXIIXE7KFJPJ3L";

unsigned long channelNumber = 1052147;


int delayTime = 1000;
uint8_t DHTPin = 13;
int timeCount = 1;

WiFiClient client;
DHT dht(DHTPin,DHTTYPE);
  void setup() {
    pinMode(sw,INPUT);
    pinMode(DHTPin,INPUT);  
    pinMode(ledRed,OUTPUT);
    pinMode(ledBlue,OUTPUT);
    pinMode(ledGreen,OUTPUT);
    Serial.begin(9600);
    wiFiConnect();
    ThingSpeak.begin(client);
    dht.begin();
  }

  void wiFiConnect(){
    WiFi.begin(ssid,password);
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.println("Connected");
    }
  }

  void loop() {
    timeCount++;
    thinkSpeakRead();
    switchTouch();
    readHumidity();
    readTemperature();
    ldrRead();
    turnLed();
    uploadClound();
    delay(delayTime);
  }

  void switchTouch(){
    int swTouch = digitalRead(sw);
    if(!ledBlueOn && swTouch == HIGH){
      turnOn = 1;
      ledBlueOn = true;
    }else if( ledBlueOn && swTouch == HIGH){
      turnOn = 0;
      ledBlueOn = false;
    }
  }
  
  void ldrRead(){
    int ldrV = analogRead(ldr);
    ldrValue = ldrV;
    if(!ledBlueOn){
      if(ldrValue  < 500){
        ledRedOn  = true;
      }else{
        ledGreenOn = true;
      }
    }
  }

  void thinkSpeakRead(){
    turnOn = ThingSpeak.readFloatField(channelNumber,TURN_ON);
    if(!ledBlueOn && turnOn == 1){
      ledRedOn = false;
      ledGreenOn = false;
      ledBlueOn = true;
    }else if(ledBlueOn && turnOn == 0){
      ledRedOn = false;
      ledGreenOn = false;
      ledBlueOn = false;
    }
    
  }

  void turnLed(){
    digitalWrite(ledBlue,LOW);
    digitalWrite(ledGreen,LOW);
    digitalWrite(ledRed,LOW);
    if(ledBlueOn){
      digitalWrite(ledBlue,HIGH);
    }else if(ledGreenOn){
      digitalWrite(ledGreen,HIGH);
    }else {
      digitalWrite(ledRed,HIGH);
    }
  }

  void readHumidity(){
    float h = dht.readHumidity();
    if(!isnan(h) ){
      humidityValue = (int)h ;
    }else{
      humidityValue = 0;
    }
  }
  
  void readTemperature(){
    float t = dht.readTemperature();
    if(!isnan(t) ){
      temperatureValue = (int)t ;
    }else{
      temperatureValue = 0;
    }
  }
  
  void uploadClound(){
    int ledOn = 0;
    if(ledRedOn){
      ledOn = 82;
    }else if(ledGreenOn){
      ledOn = 71;  
    }else if(ledBlueOn){
      ledOn = 66;
    }
    int value = map(ldrValue,0,1023,0,100);
    value = 100 - value; 
    if(timeCount == 20){
      ThingSpeak.setField(HUMIDITY,humidityValue);    
      ThingSpeak.setField(LED_ON,ledOn);
      ThingSpeak.setField(LDR,value);
      ThingSpeak.setField(TURN_ON,turnOn);
      ThingSpeak.setField(TEMPERATURE,temperatureValue);
      ThingSpeak.writeFields(channelNumber,claveApiWrite);
      timeCount = 0;
    }
  }
