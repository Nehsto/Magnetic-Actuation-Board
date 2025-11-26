#define __ASSERT_USE_STDERR
#include <Wire.h>
#include <assert.h>
#include "Magnet.h"
#include "Arduino.h" 

Magnet::Magnet(){
  voltage = 0;
  upPin = 0;
  downPin = 0;
  PWMPin = 0;
  B_field = 0;
  //Comment out if using shift registers
  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
  pinMode(PWMPin, OUTPUT);
}

Magnet::Magnet(int p, int u, int d):
  upPin(u), downPin(d), PWMPin(p){
  voltage = 0;
  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
  pinMode(PWMPin, OUTPUT);
}

void Magnet::set_voltage(float new_v){
  voltage = new_v;
  
  if(new_v == 0){
    B_field = 0; 
  }
  else if (new_v > 0){
    B_field = 1;
  }
  else if (new_v < 0){
    B_field = 2;
  }
}

void Magnet::set_intensity(float intensity){
  set_voltage(intensity*PIN_VOLTAGE/INTENSITY_RATIO);
}
