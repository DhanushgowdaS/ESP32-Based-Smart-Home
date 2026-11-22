#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// I2C address is commonly 0x27 or 0x3F; update after scanning.
constexpr uint8_t SDA_PIN=21,SCL_PIN=22,LCD_ADDR=0x27;
LiquidCrystal_I2C lcd(LCD_ADDR,16,2);
void setup(){Wire.begin(SDA_PIN,SCL_PIN);lcd.init();lcd.backlight();lcd.setCursor(0,0);lcd.print("SMART HOME");lcd.setCursor(0,1);lcd.print("LCD TEST OK");}
void loop(){}