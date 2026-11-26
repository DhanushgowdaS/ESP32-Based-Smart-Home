#include <Arduino.h>
#include <WiFi.h>
// Replace with your Wi-Fi credentials before uploading.
const char* WIFI_SSID="YOUR_WIFI_NAME";const char* WIFI_PASSWORD="YOUR_WIFI_PASSWORD";
const char* AP_SSID="SmartHome-ESP32";const char* AP_PASSWORD="smarthome123";constexpr unsigned long TIMEOUT=20000;
void setup(){Serial.begin(115200);delay(300);WiFi.mode(WIFI_AP_STA);Serial.printf("Connecting STA: %s\n",WIFI_SSID);WiFi.begin(WIFI_SSID,WIFI_PASSWORD);unsigned long t=millis();while(WiFi.status()!=WL_CONNECTED&&millis()-t<TIMEOUT){delay(500);Serial.print(".");}if(WiFi.status()==WL_CONNECTED)Serial.printf("\nSTA connected. IP=%s\n",WiFi.localIP().toString().c_str());else Serial.println("\nSTA timeout: verify credentials.");if(WiFi.softAP(AP_SSID,AP_PASSWORD))Serial.printf("AP started: SSID=%s IP=%s\n",AP_SSID,WiFi.softAPIP().toString().c_str());else Serial.println("AP start failed.");}
void loop(){}