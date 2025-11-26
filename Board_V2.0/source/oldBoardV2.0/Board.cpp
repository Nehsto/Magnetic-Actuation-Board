// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <assert.h>
#include "Magnet.h"
#include "Board.h"
#include "Arduino.h" 

Board::Board(){
  memset(board_layout, NULL, DIMENSION*DIMENSION); 
}

Board::Board(int clockPin, int latchPin[], int dataPin[]){
  memset(board_layout, NULL, DIMENSION*DIMENSION);
  for(int i = 0; i < DIMENSION*DIMENSION; i++){
    board_layout[i] = Magnet();
  }

  MSR_clockP = clockPin;
  pinMode(MSR_clockP, OUTPUT);
  for(int i = 0; i < (NUM_ROWS*NUM_COLS)/4; ++i){ 
    MSR_latchP[i] = latchPin[i];
    pinMode(MSR_latchP[i], OUTPUT);
    MSR_dataP[i] = dataPin[i];
    pinMode(MSR_dataP[i], OUTPUT);
  }
}

void Board::set(Adafruit_PWMServoDriver* Board_Ref){
  PWM_Board_ptr = Board_Ref;
}

Magnet * Board::get_magnet(int r, int c){
  return &board_layout[(r * DIMENSION) + c];
}

void Board::set_intensity(float value, int r, int c){
  Magnet* mag_ptr = get_magnet(r, c); 
  mag_ptr -> Magnet::set_intensity(value);

  uint16_t off_interval = (abs(mag_ptr->voltage)/PIN_VOLTAGE)*float(PWM_RATIO);
  
  PWM_Board_ptr -> setPWM((r*DIMENSION)+c, 0, off_interval); 
}

void Board::set_all_intensity(float values[DIMENSION*DIMENSION]){
  for(int r = 0; r < DIMENSION; r++){
    for(int c = 0; c < DIMENSION; c++){
      set_intensity(values[DIMENSION*r+c], r, c);
    }
  }
}

void Board::output_directions(){
  uint8_t block_byte;
  Magnet* mag_ptr;
  
  for(int block = 0; block < 4; block++){
    block_byte = 0b0;
    for(int mag_id = 0; mag_id < 4; mag_id++){
      //TODO: Figure out what to do with block and mag_id when board changes to 8x8
      block_byte |= (board_layout[mag_id+(4*block)].B_field & 0b11) << 2 * mag_id;    
    } 

    // Write the block byte to the shift register you want to control
    digitalWrite(MSR_latchP[block], LOW);
    shiftOut(MSR_dataP[block], MSR_clockP, MSBFIRST, block_byte & 0xFF);
    digitalWrite(MSR_latchP[block], HIGH);
  } 
}

void Board::set_zero(){
  float zero[DIMENSION*DIMENSION] = {0};
  output(zero);
}

void Board::output(float frame[DIMENSION*DIMENSION]){
  set_all_intensity(frame);
  output_directions();
}
