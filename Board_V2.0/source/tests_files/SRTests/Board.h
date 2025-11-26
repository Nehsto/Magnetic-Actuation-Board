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

class Board{
  private:
    Magnet * board_layout[DIMENSION*DIMENSION];
    Adafruit_PWMServoDriver * PWM_Board_ptr;
    //Main Shift Register pins;
    int MSR_clockP;
    int MSR_latchP;
    int MSR_dataP[DIMENSION*DIMENSION/4];
    
  public:    
  
    //Board default constructor
    Board();
    
    //Board destructor
    ~Board();
    
    //Sets up board with given pin arrays
    Board(int PWMPins[], int upPins[], int downPins[], int clockPin, int latchPin, int dataPin[DIMENSION*DIMENSION]);
    
    //== operator overloading
    operator==(const float voltages[DIMENSION*DIMENSION]);

    void set(Adafruit_PWMServoDriver* Board_Ref);
    
    //Get the pointer to a magnet at a certain position
    Magnet * get_magnet(int r, int c);
    
    // Set up the voltage for a magnet
    void set_voltage(float value, int r, int c);
    // Set up the intensity of the signal for a magnet
    
    void set_intensity(float value, int r, int c);
    // Set up the voltage for all magnets
    
    void set_all_voltages(float values[DIMENSION*DIMENSION]);
    // Set up the intensities for all magnets
    
    void set_all_intensity(float values[DIMENSION*DIMENSION]);

    void set_all_directions(bool on_shift_reg);
    void set_all_directions(int frames[DIMENSION*DIMENSION]);
    
    // Set all magnets to 0
    void set_zero();
    
    // Set the array of intensity values for the board. 
    // The vals array is multidimensional, and holds the 
    // different values to be set on the board as frames.
    // Remember to pass the amount of sub arrays for each
    void run_frames_intensity(float* vals[DIMENSION*DIMENSION], const int frames, int delayms);
    
    //print the three pins corresponding to each magnet
    void print_connections();
    
    //print the voltage for each magnet on the board
    int * print_voltages();
    
    //print the intensity of the signal for each magnet on the board
    int * print_intensity();
};

#endif
