// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#ifndef Board_h
#define Board_h
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Arduino.h" 
#include "Magnet.h"

//DIMENSIONxDIMENSION BOARD
const int DIMENSION = 4; 
//PWM Board pin
const unsigned int EOPin = 4;
const uint16_t PWM_RATIO = 4095;

class Board{
  public:
    Magnet board_layout[DIMENSION*DIMENSION];
    
    Adafruit_PWMServoDriver * PWM_Board_ptr;
       
    //Main Shift Register pins;
    int MSR_clockP;
    int MSR_latchP[DIMENSION*DIMENSION/4];
    int MSR_dataP[DIMENSION*DIMENSION/4];
     
    //Board default constructor
    Board();
    
    //Sets up board with given pin arrays
    Board(int PWMPins[], int upPins[], int downPins[], int clockPin, int latchPin[], int dataPin[DIMENSION*DIMENSION]);

    void set(Adafruit_PWMServoDriver* Board_Ref);
    
    //Get the pointer to a magnet at a certain position
    Magnet * get_magnet(int r, int c);
    
    // Set up the intensity of the signal for a magnet
    void set_intensity(int value, int r, int c);
    
    // Set up the intensities for all magnets
    void set_all_intensity(int values[DIMENSION*DIMENSION]);

    void output_directions();

    // Set all magnets to 0
    void set_zero();
    
    void output(int frame[DIMENSION*DIMENSION]);
};

#endif
