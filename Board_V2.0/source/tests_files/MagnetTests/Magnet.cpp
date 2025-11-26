#define __ASSERT_USE_STDERR
#include <assert.h>
#include "Magnet.h"
#include "Arduino.h" 

const float PIN_VOLTAGE = 5;
const float MAX_VOLTAGE = 5;
const float DIGITAL_ANALOG_CONVERSION_RATE = 255;
const float INTENSITY_RATIO = 100;

Magnet::Magnet(){
  voltage = 0;
  upPin = 0;
  downPin = 0;
  PWMPin = 0;
  active = false;
  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
  pinMode(PWMPin, OUTPUT);
}
Magnet::Magnet(int p, int u, int d):
  upPin(u), downPin(d), PWMPin(p){
  voltage = 0;
  active = false;
  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
  pinMode(PWMPin, OUTPUT);
}
int Magnet::get_PWMPin(){
  return PWMPin;
}
int Magnet::get_upPin(){
  return upPin;
}
int Magnet::get_downPin(){
  return downPin;
}
float Magnet::get_voltage(){
  return voltage;
}
float Magnet::get_intensity(){
  return voltage*INTENSITY_RATIO/PIN_VOLTAGE;
}
bool Magnet::is_active(){
  return active;
}
void Magnet::set_voltage(float new_v){
  assert(abs(new_v) <= PIN_VOLTAGE);
  voltage = new_v;
//  Serial.println(voltage);
//  Serial.println(get_voltage());
  if(new_v == 0){
    active = false;
    B_up = false;
    digitalWrite(upPin, LOW);
    digitalWrite(downPin, LOW);  
  }
  else{
    active = true;
    B_up = abs(new_v) == new_v;
    digitalWrite(upPin, B_up);
    digitalWrite(downPin, !B_up);
  }
  new_v = abs((DIGITAL_ANALOG_CONVERSION_RATE*new_v)/PIN_VOLTAGE);
  analogWrite(PWMPin, new_v);
}
void Magnet::set_intensity(float intensity){
  assert(abs(intensity) <= INTENSITY_RATIO);
  set_voltage(intensity*PIN_VOLTAGE/INTENSITY_RATIO);
}
