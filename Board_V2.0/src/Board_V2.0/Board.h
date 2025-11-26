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
    // Array holding Magnet objects
    Magnet board_layout[NUM_ROWS*NUM_COLS];

    // Declaration of pointer to access board object in main file
    Adafruit_PWMServoDriver * PWM_Board_ptr;
       
    // Shift Register pins;
    uint8_t SR_clockP;
    uint8_t SR_latchP[NUM_ROWS*NUM_COLS/4];
    uint8_t SR_dataP[NUM_ROWS*NUM_COLS/4];
     
    // Board default constructor. Needs PWM board pointer to be declared.
    Board(Adafruit_PWMServoDriver* Board_Ref);
    
    // Board constructor with given pin arrays
    Board(Adafruit_PWMServoDriver* Board_Ref, uint8_t clockPin, uint8_t latchPin[NUM_ROWS*NUM_COLS/4], uint8_t dataPin[NUM_ROWS*NUM_COLS/4]);

    // Set up board configuration and pointer to PWM board
    void set(Adafruit_PWMServoDriver* Board_Ref);
    
    //Get the pointer to a magnet at a certain position
    Magnet* get_magnet(uint8_t r, uint8_t c);
    
    // Set up the intensity of the signal for a magnet
    void set_intensity(uint8_t value, uint8_t r, uint8_t c);
    
    // Set up the intensities for all magnets
    void set_all_intensity(uint8_t values[NUM_ROWS*NUM_COLS]);

    // Used to set up shift registers according to the direction values in the board_layout array
    void output_directions();

    // Set all magnets to 0
    void set_zero();
    
    // Function used to send out a frame to the board
    void output(uint8_t frame[NUM_ROWS*NUM_COLS]);
};

#endif
