/*
  ESP32-Based Smart Home Automation — Integrated Firmware
  Board: ESP32 DevKit V1
  Relays: GPIO 23, 19, 18, 27 (assumed active LOW)
  Switches: GPIO 13, 14, 32, 33 (active LOW INPUT_PULLUP)
  LCD: 16x2 I2C, SDA 21 / SCL 22, default address 0x27
  Libraries required:
    - ArduinoJson (6.x or 7.x)
    - LiquidCrystal_I2C (compatible library)
    - ESPAsyncWebServer
    - AsyncTCP
  Dashboard files must be uploaded to LittleFS under / (index.html, css/, js/).
  IMPORTANT: Test with relay contacts disconnected from mains.
*/
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

namespace Pins {
  constexpr uint8_t RELAY[4]  = {23, 19, 18, 27};
  constexpr uint8_t SWITCH[4] = {13, 14, 32, 33};
  constexpr uint8_t SDA = 21;
  constexpr uint8_t SCL = 22;
}
constexpr bool RELAY_ACTIVE_LOW = true;
constexpr uint8_t LCD_ADDR = 0x27;
constexpr uint32_t DEBOUNCE_MS = 45;
const char* AP_NAME = "SmartHome-ESP32";
const char* AP_PASS = "smarthome123"; // Change before public use
const char* STA_SSID = "YOUR_WIFI_NAME";
const char* STA_PASS = "YOUR_WIFI_PASSWORD";
const char* CONFIG_FILE = "/config.json";

LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool relayState[4] = {false,false,false,false};
bool rawSwitch[4] = {HIGH,HIGH,HIGH,HIGH};
bool stableSwitch[4] = {HIGH,HIGH,HIGH,HIGH};
uint32_t switchChangedAt[4] = {0,0,0,0};
String applianceName[4] = {"Red Lamp","Green Lamp","Blue Lamp","Yellow Lamp"};
String applianceType[4] = {"Light","Light","Light","Light"};
String applianceIcon[4] = {"bulb","bulb","bulb","bulb"};
String applianceColor[4] = {"#ff5b61","#27d17f","#438cff","#f1c232"};
String lcdStatus = "Starting...";
uint32_t lcdLastUpdate = 0;
uint8_t lcdPage = 0;

String jsonState() {
  JsonDocument doc;
  JsonArray channels = doc["channels"].to<JsonArray>();
  for (uint8_t i=0;i<4;i++) channels.add(relayState[i]);
  doc["wifi"] = WiFi.status()==WL_CONNECTED ? "connected" : "ap";
  doc["ip"] = WiFi.status()==WL_CONNECTED ? WiFi.localIP().toString() : WiFi.softAPIP().toString();
  String out; serializeJson(doc,out); return out;
}
String channelJson(uint8_t ch) {
  JsonDocument doc;
  doc["channel"]=ch+1; doc["state"]=relayState[ch];
  String out; serializeJson(doc,out); return out;
}
void publishState(uint8_t ch) {
  ws.textAll(channelJson(ch));
}
void setRelay(uint8_t ch, bool on, bool broadcast=true) {
  if(ch>=4) return;
  relayState[ch]=on;
  digitalWrite(Pins::RELAY[ch], RELAY_ACTIVE_LOW ? (on?LOW:HIGH) : (on?HIGH:LOW));
  Serial.printf("Relay/channel %u => %s\n",ch+1,on?"ON":"OFF");
  if(broadcast) publishState(ch);
}
void allOff() { for(uint8_t i=0;i<4;i++) setRelay(i,false); }
void loadConfig() {
  if(!LittleFS.exists(CONFIG_FILE)) return;
  File f=LittleFS.open(CONFIG_FILE,"r"); if(!f) return;
  JsonDocument doc; DeserializationError e=deserializeJson(doc,f); f.close();
  if(e){Serial.println("Config JSON invalid; using defaults.");return;}
  JsonArray arr=doc["channels"].as<JsonArray>();
  for(uint8_t i=0;i<4 && i<arr.size();i++){
    JsonObject c=arr[i];
    if(c["name"].is<const char*>()) applianceName[i]=String((const char*)c["name"]);
    if(c["type"].is<const char*>()) applianceType[i]=String((const char*)c["type"]);
    if(c["icon"].is<const char*>()) applianceIcon[i]=String((const char*)c["icon"]);
    if(c["color"].is<const char*>()) applianceColor[i]=String((const char*)c["color"]);
  }
}
bool saveConfig(JsonDocument& doc) {
  File f=LittleFS.open(CONFIG_FILE,"w"); if(!f) return false;
  bool ok=serializeJson(doc,f)>0; f.close(); return ok;
}
void configJson(String& out) {
  JsonDocument doc; JsonArray arr=doc["channels"].to<JsonArray>();
  for(uint8_t i=0;i<4;i++){
    JsonObject c=arr.add<JsonObject>();
    c["id"]=i+1; c["name"]=applianceName[i]; c["type"]=applianceType[i];
    c["icon"]=applianceIcon[i]; c["color"]=applianceColor[i]; c["state"]=relayState[i];
  }
  serializeJson(doc,out);
}
void lcdPrintLine(uint8_t row,String text) {
  if(text.length()>16) text=text.substring(0,16);
  while(text.length()<16) text+=" ";
  lcd.setCursor(0,row); lcd.print(text);
}
void updateLCD() {
  if(millis()-lcdLastUpdate<2500) return;
  lcdLastUpdate=millis();
  if(WiFi.status()==WL_CONNECTED) {
    if(lcdPage==0){lcdPrintLine(0,"Wi-Fi Connected");lcdPrintLine(1,WiFi.localIP().toString());}
    else {lcdPrintLine(0,"Smart Home");lcdPrintLine(1,String("ON:")+String((relayState[0]?1:0)+(relayState[1]?1:0)+(relayState[2]?1:0)+(relayState[3]?1:0))+"  AP ready");}
  } else {
    if(lcdPage==0){lcdPrintLine(0,"Wi-Fi: AP Mode");lcdPrintLine(1,WiFi.softAPIP().toString());}
    else {lcdPrintLine(0,"Connect to:");lcdPrintLine(1,AP_NAME);}
  }
  lcdPage=(lcdPage+1)%2;
}
void handleSwitches() {
  uint32_t now=millis();
  for(uint8_t i=0;i<4;i++){
    bool reading=digitalRead(Pins::SWITCH[i]);
    if(reading!=rawSwitch[i]){rawSwitch[i]=reading;switchChangedAt[i]=now;}
    if(now-switchChangedAt[i]>=DEBOUNCE_MS && stableSwitch[i]!=rawSwitch[i]){
      stableSwitch[i]=rawSwitch[i];
      // Toggle on a debounced switch press/active edge (LOW).
      if(stableSwitch[i]==LOW) setRelay(i,!relayState[i]);
    }
  }
}
void sendStatus(AsyncWebServerRequest* req) {
  req->send(200,"application/json",jsonState());
}
void setupRoutes() {
  server.on("/",HTTP_GET,[](AsyncWebServerRequest* r){ if(LittleFS.exists("/index.html"))r->send(LittleFS,"/index.html","text/html");else r->send(503,"text/plain","Dashboard files missing in LittleFS");});
  server.on("/api/status",HTTP_GET,sendStatus);
  server.on("/api/config",HTTP_GET,[](AsyncWebServerRequest* r){String out;configJson(out);r->send(200,"application/json",out);});
  server.on("/api/config",HTTP_POST,[](AsyncWebServerRequest* r){r->send(400,"application/json","{\"error\":\"Send JSON body\"}");},nullptr,
  [](AsyncWebServerRequest* r,uint8_t* data,size_t len,size_t index,size_t total){
    if(index==0){r->_tempObject=new String();}
    String* body=(String*)r->_tempObject; body->concat((const char*)data,len);
    if(index+len==total){
      JsonDocument doc; DeserializationError e=deserializeJson(doc,*body);
      if(e){r->send(400,"application/json","{\"error\":\"Invalid JSON\"}");delete body;r->_tempObject=nullptr;return;}
      JsonArray arr=doc["channels"].as<JsonArray>();
      if(arr.size()!=4){r->send(400,"application/json","{\"error\":\"Expected four channel settings\"}");delete body;r->_tempObject=nullptr;return;}
      for(uint8_t i=0;i<4;i++){JsonObject c=arr[i];applianceName[i]=String((const char*)(c["name"]|"Channel"));applianceType[i]=String((const char*)(c["type"]|"Other"));applianceIcon[i]=String((const char*)(c["icon"]|"home"));applianceColor[i]=String((const char*)(c["color"]|"#ffffff"));}
      bool ok=saveConfig(doc);r->send(ok?200:500,"application/json",ok?"{\"saved\":true}":"{\"error\":\"Filesystem write failed\"}");
      delete body;r->_tempObject=nullptr;
    }
  });
  for(uint8_t i=0;i<4;i++){
    String path="/api/relay/"+String(i+1);
    server.on(path.c_str(),HTTP_POST,[i](AsyncWebServerRequest* r){r->send(400,"application/json","{\"error\":\"Send JSON body\"}");},nullptr,
    [i](AsyncWebServerRequest* r,uint8_t* data,size_t len,size_t index,size_t total){
      String* body=(String*)r->_tempObject;if(index==0){body=new String();r->_tempObject=body;}body->concat((const char*)data,len);
      if(index+len==total){JsonDocument doc;auto e=deserializeJson(doc,*body);if(e||!doc["state"].is<bool>()){r->send(400,"application/json","{\"error\":\"state boolean required\"}");}else{setRelay(i,doc["state"].as<bool>());r->send(200,"application/json",channelJson(i));}delete body;r->_tempObject=nullptr;}
    });
  }
  server.on("/api/all-off",HTTP_POST,[](AsyncWebServerRequest* r){allOff();r->send(200,"application/json",jsonState());});
  server.onNotFound([](AsyncWebServerRequest* r){if(r->method()==HTTP_OPTIONS){r->send(204);}else if(LittleFS.exists(r->url()))r->send(LittleFS,r->url(),"text/plain");else r->send(404,"text/plain","Not found");});
  ws.onEvent([](AsyncWebSocket* server,AsyncWebSocketClient* client,AwsEventType type,void* arg,uint8_t* data,size_t len){
    if(type==WS_EVT_CONNECT){client->text(jsonState());}
  });
  server.addHandler(&ws);
}
void setup() {
  Serial.begin(115200);delay(250);
  // Initialize relays in OFF state before other setup.
  for(uint8_t i=0;i<4;i++){digitalWrite(Pins::RELAY[i],RELAY_ACTIVE_LOW?HIGH:LOW);pinMode(Pins::RELAY[i],OUTPUT);relayState[i]=false;}
  for(uint8_t i=0;i<4;i++)pinMode(Pins::SWITCH[i],INPUT_PULLUP);
  Wire.begin(Pins::SDA,Pins::SCL);
  lcd.init();lcd.backlight();lcd.clear();lcdPrintLine(0,"Smart Home");lcdPrintLine(1,"Starting...");
  if(!LittleFS.begin(false)){Serial.println("LittleFS mount failed (no auto format).");lcdPrintLine(0,"LittleFS Error");lcdPrintLine(1,"Dashboard missing");}else loadConfig();
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_NAME,AP_PASS);
  lcdPrintLine(0,"Waiting Wi-Fi...");
  WiFi.begin(STA_SSID,STA_PASS);
  uint32_t start=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-start<20000){delay(400);Serial.print(".");}
  if(WiFi.status()==WL_CONNECTED){Serial.printf("\nWi-Fi connected. IP: %s\n",WiFi.localIP().toString().c_str());lcdPrintLine(0,"Wi-Fi Connected");lcdPrintLine(1,WiFi.localIP().toString());}
  else {Serial.println("\nWi-Fi timeout; AP mode remains available.");lcdPrintLine(0,"AP Mode Ready");lcdPrintLine(1,WiFi.softAPIP().toString());}
  Serial.printf("AP SSID: %s | AP IP: %s\n",AP_NAME,WiFi.softAPIP().toString().c_str());
  setupRoutes();server.begin();Serial.println("HTTP server started (port 80).");
}
void loop(){handleSwitches();updateLCD();ws.cleanupClients();delay(2);}
