#include <Arduino.h>
#include <WiFi.h>
const char* SSID="YOUR_WIFI_NAME"; const char* PASSWORD="YOUR_WIFI_PASSWORD";
const char* AP_SSID="SmartHome-ESP32"; const char* AP_PASS="smarthome123";
void setup(){Serial.begin(115200);WiFi.mode(WIFI_AP_STA);WiFi.begin(SSID,PASSWORD);unsigned long t=millis();while(WiFi.status()!=WL_CONNECTED&&millis()-t<15000){delay(300);Serial.print(".");}if(WiFi.status()==WL_CONNECTED)Serial.printf("\nSTA IP: %s\n",WiFi.localIP().toString().c_str());else Serial.println("\nSTA not connected.");if(WiFi.softAP(AP_SSID,AP_PASS))Serial.printf("AP SSID: %s | IP: %s\n",AP_SSID,WiFi.softAPIP().toString().c_str());else Serial.println("AP failed.");}
void loop(){}