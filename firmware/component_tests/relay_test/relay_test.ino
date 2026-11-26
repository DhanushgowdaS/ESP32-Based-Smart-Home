#include <Arduino.h>
// Relay test only. Keep mains disconnected. Confirm trigger polarity first.
constexpr uint8_t RELAY_PINS[4]={23,19,18,27};
constexpr bool ACTIVE_LOW=true;
bool states[4]={false,false,false,false};
void setRelay(uint8_t ch,bool on){if(ch>=4)return;states[ch]=on;digitalWrite(RELAY_PINS[ch],ACTIVE_LOW?(on?LOW:HIGH):(on?HIGH:LOW));Serial.printf("Relay %u -> %s\n",ch+1,on?"ON":"OFF");}
void help(){Serial.println("\nCommands: 1ON/1OFF, 2ON/2OFF, 3ON/3OFF, 4ON/4OFF, ALLON, ALLOFF, STATUS");}
void setup(){Serial.begin(115200);Serial.setTimeout(100);for(uint8_t i=0;i<4;i++){digitalWrite(RELAY_PINS[i],ACTIVE_LOW?HIGH:LOW);pinMode(RELAY_PINS[i],OUTPUT);}for(uint8_t i=0;i<4;i++)setRelay(i,false);help();}
void loop(){if(!Serial.available())return;String c=Serial.readStringUntil('\n');c.trim();c.toUpperCase();if(c=="ALLON"){for(uint8_t i=0;i<4;i++)setRelay(i,true);return;}if(c=="ALLOFF"){for(uint8_t i=0;i<4;i++)setRelay(i,false);return;}if(c=="STATUS"){for(uint8_t i=0;i<4;i++)Serial.printf("Relay %u: %s\n",i+1,states[i]?"ON":"OFF");return;}if(c.length()==3&&c[0]>='1'&&c[0]<='4'&&(c.endsWith("ON")||c.endsWith("FF"))){setRelay(c[0]-'1',c.endsWith("ON"));return;}Serial.println("Invalid command. Type the command list.");}