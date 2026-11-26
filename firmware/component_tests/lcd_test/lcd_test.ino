#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Use I2C scanner to confirm address. Common addresses: 0x27 and 0x3F.
constexpr uint8_t SDA_PIN=21,SCL_PIN=22,LCD_ADDRESS=0x27;LiquidCrystal_I2C lcd(LCD_ADDRESS,16,2);
void setup(){Serial.begin(115200);Wire.begin(SDA_PIN,SCL_PIN);lcd.init();lcd.backlight();lcd.clear();lcd.setCursor(0,0);lcd.print("SMART HOME");lcd.setCursor(0,1);lcd.print("LCD TEST OK");Serial.println("LCD test started; confirm address and contrast if blank.");}
void loop(){}