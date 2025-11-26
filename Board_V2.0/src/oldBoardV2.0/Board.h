// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#ifndef Board_h
#define Board_h
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Arduino.h" 
#include "Magnet.h"

//NUM_ROWS x NUM_COLS BOARD
const uint8_t NUM_ROWS = 4;
const uint8_t NUM_COLS = 4; 
//PWM Board pin
const uint8_t EOPin = 4;
const uint16_t PWM_RATIO = 4095;

class Board{
  public:
    Magnet board_layout[NUM_ROWS*NUM_COLS];
    
    Adafruit_PWMServoDriver * PWM_Board_ptr;
       
    //Main Shift Register pins;
    int MSR_clockP;
    int MSR_latchP[NUM_ROWS*NUM_COLS/4];
    int MSR_dataP[NUM_ROWS*NUM_COLS/4];
     
    //Board default constructor
    Board();
    
    //Sets up board with given pin arrays
    Board(int clockPin, int latchPin[], int dataPin[NUM_ROWS*NUM_COLS]);

    void set(Adafruit_PWMServoDriver* Board_Ref);
    
    //Get the pointer to a magnet at a certain position
    Magnet * get_magnet(int r, int c);
    
    // Set up the intensity of the signal for a magnet
    void set_intensity(float value, int r, int c);
    
    // Set up the intensities for all magnets
    void set_all_intensity(float values[NUM_ROWS*NUM_COLS]);

    // Used to set up shift registers according to the direction values in the board_layout array
    void output_directions();

    // Set all magnets to 0
    void set_zero();
    
    // Function used to send out a frame to the board
    void output(float frame[NUM_ROWS*NUM_COLS]);
};

#endif
