// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#include <Wire.h>
#include <assert.h>
#include "Magnet.h"
#include "Arduino.h" 

Magnet::Magnet(){
  voltage = 0;
  B_dir = 0;
}

void Magnet::set_voltage(float new_v){
  voltage = new_v;
  
  if(new_v == 0){
    B_dir = 0b00; 
  }
  else{
    if (new_v > 0){
      B_dir = 0b01;
    }
    else{
      B_dir = 0b10;
    }
  }
}

void Magnet::set_intensity(uint8_t intensity){
  set_voltage(intensity*PIN_VOLTAGE/INTENSITY_RATIO);
}
