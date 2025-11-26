// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <assert.h>
#include "Magnet.h"
#include "Board.h"
#include "Arduino.h" 

Board::Board(Adafruit_PWMServoDriver* Board_Ref){
  memset(board_layout, NULL, NUM_ROWS*NUM_COLS); 
  PWM_Board_ptr = Board_Ref;
}

Board::Board(Adafruit_PWMServoDriver* Board_Ref, uint8_t clockPin, uint8_t latchPin[NUM_ROWS*NUM_COLS/4], uint8_t dataPin[NUM_ROWS*NUM_COLS/4]){
  memset(board_layout, NULL, NUM_ROWS*NUM_COLS);
  for(uint8_t i = 0; i < NUM_ROWS*NUM_COLS; i++){
    board_layout[i] = Magnet();
  }

  SR_clockP = clockPin;
  pinMode(SR_clockP, OUTPUT);
  for(uint8_t i = 0; i < (NUM_ROWS*NUM_COLS)/4; ++i){ 
    SR_latchP[i] = latchPin[i];
    pinMode(SR_latchP[i], OUTPUT);
    SR_dataP[i] = dataPin[i];
    pinMode(SR_dataP[i], OUTPUT);
  }
  PWM_Board_ptr = Board_Ref;
}

void Board::set(Adafruit_PWMServoDriver* Board_Ref){
  PWM_Board_ptr = Board_Ref;
}

Magnet* Board::get_magnet(uint8_t r, uint8_t c){
  return &board_layout[r * NUM_COLS + c];
}

void Board::set_intensity(uint8_t value, uint8_t r, uint8_t c){
  Magnet* mag_ptr = get_magnet(r, c); 
  mag_ptr -> Magnet::set_intensity(value);

  uint16_t off_interval = (abs(mag_ptr->voltage)/PIN_VOLTAGE)*float(PWM_RATIO);
  
  PWM_Board_ptr -> setPWM(r * NUM_COLS + c, 0, off_interval); 
}

void Board::set_all_intensity(uint8_t values[NUM_ROWS*NUM_COLS]){
  for(uint8_t r = 0; r < NUM_ROWS; r++){
    for(uint8_t c = 0; c < NUM_COLS; c++){
      set_intensity(values[r * NUM_COLS + c], r, c);
    }
  }
}

void Board::output_directions(){
  uint8_t block_byte;
  Magnet* mag_ptr;
  
  for(uint8_t block = 0; block < 4; block++){
    block_byte = 0b0;
    for(uint8_t mag_id = 0; mag_id < 4; mag_id++){
      //TODO: Figure out what to do with block and mag_id when board changes to different dimensions
      block_byte |= (board_layout[mag_id+(4*block)].B_dir & 0b11) << 2 * mag_id;    
    } 

    // Write the block byte to the shift register you want to control
    digitalWrite(SR_latchP[block], LOW);
    shiftOut(SR_dataP[block], SR_clockP, MSBFIRST, block_byte & 0xFF);
    digitalWrite(SR_latchP[block], HIGH);
  } 
}

void Board::set_zero(){
  uint8_t zero[NUM_ROWS*NUM_COLS] = {0};
  output(zero);
}

void Board::output(uint8_t frame[NUM_ROWS*NUM_COLS]){
  set_all_intensity(frame);
  output_directions();
}
