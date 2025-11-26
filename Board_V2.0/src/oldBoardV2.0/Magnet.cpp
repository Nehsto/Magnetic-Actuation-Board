// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

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
}

void Magnet::set_voltage(float new_v){
  voltage = new_v;
  
  if(new_v == 0){
    B_field = 0b00; 
  }
  else{
    if (new_v > 0){
      B_field = 0b01;
    }
    else{
      B_field = 0b10;
    }
  }
}

void Magnet::set_intensity(uint8_t intensity){
  set_voltage(intensity*PIN_VOLTAGE/INTENSITY_RATIO);
}
