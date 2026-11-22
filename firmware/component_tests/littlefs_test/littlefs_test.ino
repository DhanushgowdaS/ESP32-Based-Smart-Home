#include <Arduino.h>
#include <LittleFS.h>
void setup(){Serial.begin(115200);if(!LittleFS.begin(true)){Serial.println("LittleFS mount failed");return;}File f=LittleFS.open("/test.txt","w");if(!f){Serial.println("Open for write failed");return;}f.println("Smart Home LittleFS OK");f.close();f=LittleFS.open("/test.txt","r");if(!f){Serial.println("Open for read failed");return;}while(f.available())Serial.write(f.read());f.close();}
void loop(){}