#include <Arduino.h>
// Disconnect mains loads during this test. Confirm relay polarity first.
constexpr uint8_t RELAY_PINS[4]={23,19,18,27};
constexpr bool ACTIVE_LOW=true; // Set false only if verified for your board.
void setRelay(uint8_t c,bool on){if(c>=4)return; digitalWrite(RELAY_PINS[c],(on==ACTIVE_LOW)?LOW:HIGH); Serial.printf("Relay %u: %s\n",c+1,on?"ON":"OFF");}
void setup(){Serial.begin(115200);for(uint8_t i=0;i<4;i++){digitalWrite(RELAY_PINS[i],ACTIVE_LOW?HIGH:LOW);pinMode(RELAY_PINS[i],OUTPUT);}for(uint8_t i=0;i<4;i++)setRelay(i,false);Serial.println("Commands: 1ON/1OFF ... 4ON/4OFF, ALLON, ALLOFF");}
void loop(){if(!Serial.available())return;String s=Serial.readStringUntil('\n');s.trim();s.toUpperCase();if(s=="ALLON"){for(uint8_t i=0;i<4;i++)setRelay(i,true);return;}if(s=="ALLOFF"){for(uint8_t i=0;i<4;i++)setRelay(i,false);return;}if(s.length()==3&&s[0]>='1'&&s[0]<='4'){uint8_t c=s[0]-'1';if(s.endsWith("ON"))setRelay(c,true);else if(s.endsWith("FF"))setRelay(c,false);else Serial.println("Use 1ON or 1OFF.");}else Serial.println("Unknown command.");}