// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#ifndef Magnet_h
#define Magnet_h
#include <Wire.h>
#include "Arduino.h" 

const float PIN_VOLTAGE = 5;
const float DIGITAL_ANALOG_CONVERSION_RATE = 255;
const float INTENSITY_RATIO = 100;

class Magnet{  
  public:
    float voltage;
    unsigned int B_field; //Magnetic field pointing up
    int PWMPin;  //Goes on bottom of the board
    int upPin;   //Control the direction of the magnetic  
    int downPin; //field at the center of the coil
    
    Magnet();
    
    Magnet(int u, int d, int p);
    
    void set_voltage(float v);
    
    // With a value -100 < intensity < 100, set up the voltage of the magnet
    void set_intensity(int intensity);
};

#endif
