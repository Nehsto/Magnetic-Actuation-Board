#ifndef Board_h
#define Board_h
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Arduino.h" 
#include "Magnet.h"

//DIMENSIONxDIMENSION BOARD
const int DIMENSION = 2; 
//PWM Board pin
const unsigned int EOPin = 4;
const float PWM_RATIO = 4096;

class Board{
  private:
    
    // float *PWMv[DIMENSION*DIMENSION];
    // int * dir[DIMENSION*DIMENSION];
    Adafruit_PWMServoDriver * PWM_Board_ptr;
       
    //Main Shift Register pins;
    int MSR_clockP;
    int MSR_latchP;
    int MSR_dataP[DIMENSION*DIMENSION/4];
    
  public:    

    Magnet board_layout[DIMENSION*DIMENSION];
    //Board default constructor
    Board();
    
    //Board destructor
    //~Board();
    
    //Sets up board with given pin arrays
    Board(int PWMPins[], int upPins[], int downPins[], int clockPin, int latchPin, int dataPin[DIMENSION*DIMENSION]);

    void set(Adafruit_PWMServoDriver* Board_Ref);
    
    //Get the pointer to a magnet at a certain position
    Magnet * get_magnet(int r, int c);
    
    // Set up the voltage for a magnet
    // void set_voltage(float value, int r, int c);
    
    // Set up the intensity of the signal for a magnet
    void set_intensity(float value, int r, int c);
    
    // Set up the voltage for all magnets
    // void set_all_voltages(float values[DIMENSION*DIMENSION]);
    
    // Set up the intensities for all magnets
    void set_all_intensity(float values[DIMENSION*DIMENSION]);

    void output_directions(bool on_shift_reg);
    //void output_directions(int frames[DIMENSION*DIMENSION]);

    // Set all magnets to 0
    void set_zero();
    
    void output(float frame[DIMENSION*DIMENSION]);
};

#endif
