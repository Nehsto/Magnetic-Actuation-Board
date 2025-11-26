#ifndef Magnet_h
#define Magnet_h
#include "Arduino.h" 

class Magnet{
  public:
    Magnet();
    Magnet(int u, int d, int p);
    int get_PWMPin();
    int get_upPin();
    int get_downPin();
    float get_voltage();
    float get_intensity();
    bool is_active();
    void set_voltage(float v);
    //with a value -100 < intensity < 100, set up the voltage of the magnet
    void set_intensity(float intensity);
    
  private:
    int PWMPin;  //Goes on bottom of the board
    int upPin;   //Control the direction of the magnetic  
    int downPin; //field at the center of the coil
    float voltage;
    bool active;
    bool B_up;      //Magnetic field pointing up
};

#endif
