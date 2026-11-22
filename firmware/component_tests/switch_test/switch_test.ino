#include <Arduino.h>
constexpr uint8_t PINS[4]={13,14,32,33};
constexpr unsigned long DEBOUNCE_MS=35;
uint8_t rawLast[4]={HIGH,HIGH,HIGH,HIGH},stable[4]={HIGH,HIGH,HIGH,HIGH};
unsigned long changed[4]={0,0,0,0};
void setup(){Serial.begin(115200);for(auto p:PINS)pinMode(p,INPUT_PULLUP);Serial.println("Switch test ready; switches connect GPIO to GND.");}
void loop(){unsigned long now=millis();for(uint8_t i=0;i<4;i++){uint8_t r=digitalRead(PINS[i]);if(r!=rawLast[i]){rawLast[i]=r;changed[i]=now;}if(now-changed[i]>=DEBOUNCE_MS&&stable[i]!=r){stable[i]=r;Serial.printf("S%u %s\n",i+1,r==LOW?"PRESSED":"RELEASED");}}}