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
    int B_field; // If magnetic field pointing up B_field = 1
                 // If pointing down, B_field = 2
                 // If zero, B_field = 0 
    int PWMPin;  //Goes to H-Bridge Enable pin to control intensity
    
    int upPin;   // Both control the direction of the magnetic  
    int downPin; // field at the center of the coil
    
    Magnet();
    
    Magnet(int u, int d, int p);
    
    void set_voltage(float v);
    
    // With a value -100 < intensity < 100, set up the voltage of the magnet
    void set_intensity(float intensity);
};

#endif
