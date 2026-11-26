#include <Arduino.h>
#include <Wire.h>
constexpr uint8_t SDA_PIN=21,SCL_PIN=22;
void setup(){Serial.begin(115200);Wire.begin(SDA_PIN,SCL_PIN);delay(300);Serial.println("\nI2C scan start");uint8_t found=0;for(uint8_t address=1;address<127;address++){Wire.beginTransmission(address);uint8_t error=Wire.endTransmission();if(error==0){Serial.printf("Found I2C device at 0x%02X\n",address);found++;}delay(2);}if(found==0)Serial.println("No I2C devices found. Check wiring, power and SDA/SCL.");else Serial.printf("Scan complete: %u device(s).\n",found);}
void loop(){}