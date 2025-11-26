#define __ASSERT_USE_STDERR
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <assert.h>
#include "Magnet.h"
#include "Arduino.h" 

Magnet::Magnet(){
  voltage = 0;
  upPin = 0;
  downPin = 0;
  PWMPin = 0;
  active = false;
  B_field = 0;
  //Comment out if using shift registers
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
int Magnet::get_PWMPin()const{
  return PWMPin;
}
int Magnet::get_upPin()const{
  return upPin;
}
int Magnet::get_downPin()const{
  return downPin;
}
float Magnet::get_voltage()const{
  return voltage;
}
float Magnet::get_intensity()const{
  return voltage*INTENSITY_RATIO/PIN_VOLTAGE;
}
int Magnet::get_B_field()const{
  return B_field;
}
bool Magnet::is_active()const{
  return active;
}
void Magnet::set_voltage(float new_v, Adafruit_PWMServoDriver *pwm_board){
  float temp = abs((new_v*PWM_RATIO)/PIN_VOLTAGE);
  unsigned int on_val = temp;
  unsigned int off_val = 0;
  voltage = new_v;
  
  if(new_v == 0){
    active = false;
    B_field = 0; 
  }
  else{
    active = true;
    if (new_v > 0){
      B_field = 1;
    }
    else{
      B_field = 2;
    }
  }
  pwm_board->setPWM(PWMPin, on_val, off_val);
}
void Magnet::set_intensity(float intensity, Adafruit_PWMServoDriver *pwm_board){
  set_voltage(intensity*PIN_VOLTAGE/INTENSITY_RATIO, pwm_board);
}
