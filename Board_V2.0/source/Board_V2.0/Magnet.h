// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#ifndef Magnet_h
#define Magnet_h
#include <Wire.h>
#include "Arduino.h" 

const float PIN_VOLTAGE     = 5.0;
const float INTENSITY_RATIO = 100;

class Magnet{  
  public:
    // Voltage set across magnet calculated with PIN_VOLTAGE
    float voltage;
    // Direction of magnetic field, only interested in first 2 bits
    // 00 - No magnetic field output
    // 01 - Positive direction
    // 10 - Negative direction
    uint8_t B_dir;
    
    Magnet();
    
    void set_voltage(float v);
    
    // With a value -100 < intensity < 100, set up the voltage of the magnet
    void set_intensity(uint8_t intensity);
};

#endif
