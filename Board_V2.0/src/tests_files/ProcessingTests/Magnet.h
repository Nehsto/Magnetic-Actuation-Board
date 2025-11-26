#ifndef Magnet_h
#define Magnet_h
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Arduino.h" 

const float PIN_VOLTAGE = 5;
const float DIGITAL_ANALOG_CONVERSION_RATE = 255;
const float PWM_RATIO = 4096;
const float INTENSITY_RATIO = 100;

class Magnet{
  private:
    int PWMPin;  //Goes on bottom of the board
    int upPin;   //Control the direction of the magnetic  
    int downPin; //field at the center of the coil
    float voltage;
    bool active;
    int B_field;      //Magnetic field pointing up
  
  public:
    Magnet();
    Magnet(int u, int d, int p);
    int get_PWMPin()const;
    int get_upPin()const;
    int get_downPin()const;
    float get_voltage()const;
    float get_intensity()const;
    int get_B_field()const;
    bool is_active()const;
    void set_voltage(float v, Adafruit_PWMServoDriver *pwm_board);
    //with a value -100 < intensity < 100, set up the voltage of the magnet
    void set_intensity(float intensity, Adafruit_PWMServoDriver *pwm_board);
};

#endif
