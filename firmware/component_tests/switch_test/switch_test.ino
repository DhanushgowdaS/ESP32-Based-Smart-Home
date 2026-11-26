#include <Arduino.h>
constexpr uint8_t SWITCH_PINS[4]={13,14,32,33};constexpr unsigned long DEBOUNCE_MS=40;
uint8_t raw[4]={HIGH,HIGH,HIGH,HIGH},stable[4]={HIGH,HIGH,HIGH,HIGH};unsigned long changed[4]={0,0,0,0};
void setup(){Serial.begin(115200);for(uint8_t i=0;i<4;i++)pinMode(SWITCH_PINS[i],INPUT_PULLUP);Serial.println("Switch test: each switch connects GPIO to GND.");}
void loop(){unsigned long now=millis();for(uint8_t i=0;i<4;i++){uint8_t r=digitalRead(SWITCH_PINS[i]);if(r!=raw[i]){raw[i]=r;changed[i]=now;}if(now-changed[i]>=DEBOUNCE_MS&&stable[i]!=raw[i]){stable[i]=raw[i];Serial.printf("S%u: %s\n",i+1,r==LOW?"PRESSED/ACTIVE":"RELEASED/INACTIVE");}}}