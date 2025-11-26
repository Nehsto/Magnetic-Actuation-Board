// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#define __ASSERT_USE_STDERR
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <assert.h>
#include "Magnet.h"
#include "Board.h"
#include "Arduino.h" 

Board::Board(){
  memset(board_layout, NULL, DIMENSION*DIMENSION); 
}

Board::Board(int PWMPins[], int upPins[], int downPins[], int clockPin, int latchPin[], int dataPin[]){
  memset(board_layout, NULL, DIMENSION*DIMENSION);
  for(int i = 0; i < DIMENSION*DIMENSION; i++){
    board_layout[i] = Magnet(PWMPins[i], upPins[i], downPins[i]);
  }

  MSR_clockP = clockPin;

  pinMode(MSR_clockP, OUTPUT);
  for(int i = 0; i < (DIMENSION*DIMENSION)/4; ++i){ //TODO: CHANGE THE 4 TO A NUMBER RELATIVE TO THE DIMENSION
    MSR_latchP[i] = latchPin[i];
    pinMode(MSR_latchP[i], OUTPUT);
    MSR_dataP[i] = dataPin[i];
    pinMode(MSR_dataP[i], OUTPUT);
  }
}

void Board::set(Adafruit_PWMServoDriver* Board_Ref, Stream* Serial_ref){
  PWM_Board_ptr = Board_Ref;
  Serial_ptr = Serial_ref;
}

Magnet * Board::get_magnet(int r, int c){
  return &board_layout[(r * DIMENSION) + c];
}

//void Board::set_voltage(float value, int r, int c){
//  if(-5.0 > value || value > 5.0) return;
//  get_magnet(r, c)-> set_voltage(value);
//}

void Board::set_intensity(float value, int r, int c){
  Magnet* mag_ptr = get_magnet(r, c); 
  
  // Magnet * ptr = 0x10;

  // (* ptr) . value 
  // ptr->value

  mag_ptr -> Magnet::set_intensity(value);
  //uint16_t((abs(mag_ptr->voltage)/PIN_VOLTAGE)*PWM_RATIO)-1);
  
  PWM_Board_ptr->setPWM((r*DIMENSION)+c, 0, uint16_t((abs(mag_ptr->voltage)/PIN_VOLTAGE)*PWM_RATIO)-1); 
  if(value != 0){
    Serial_ptr->print("Writing to board value: ");
    Serial_ptr->print(value);
    Serial_ptr->print(" Translates to ");
    Serial_ptr->println(((abs(mag_ptr->voltage)/PIN_VOLTAGE)*PWM_RATIO)-1);
  }
  // Setting with value:
  //    uint16_t((abs(value)/100.0)*PWM_RATIO)-1
  // Setting with mag voltage:
  //    uint16_t((abs(mag_ptr->voltage)/PIN_VOLTAGE)*PWM_RATIO))-1
}

//void Board::set_all_voltages(float values[DIMENSION*DIMENSION]){
//  for(int r = 0; r < DIMENSION; r++){
//    for(int c = 0; c < DIMENSION; c++){
//      set_voltage(values[DIMENSION*r+c], r, c);
//    }
//  }
//}

void Board::set_all_intensity(float values[DIMENSION*DIMENSION]){
  for(int r = 0; r < DIMENSION; r++){
    for(int c = 0; c < DIMENSION; c++){
      set_intensity(values[DIMENSION*r+c], r, c);
    }
  }
}

void Board::output_directions(){
  unsigned int block_byte;
  Magnet* mag_ptr;
  
  for(int block = 0; block < 4; block++){
    block_byte = 0b0;
    for(int mag_id = 0; mag_id < 4; mag_id++){
      //TODO: Figure out what to do with block and mag_id when board changes to 8x8
      block_byte |= (board_layout[mag_id+(4*block)].B_field & 0b11) << 2 * mag_id;    
    } 

    //DEBUG SECTION
    // digitalWrite(29, block_byte & 0b1);
    // digitalWrite(28, (block_byte >> 1) & 0b1);
    // digitalWrite(27, (block_byte >> 2) & 0b1);
    // digitalWrite(26, (block_byte >> 3) & 0b1);
    // digitalWrite(25, (block_byte >> 4) & 0b1);
    // digitalWrite(24, (block_byte >> 5) & 0b1);
    // digitalWrite(23, (block_byte >> 6) & 0b1);
    // digitalWrite(22, (block_byte >> 7) & 0b1);

    // Write the block byte to the shift register you want to control
    digitalWrite(MSR_latchP[block], LOW);
    shiftOut(MSR_dataP[block], MSR_clockP, MSBFIRST, block_byte & 0xFF);
    digitalWrite(MSR_latchP[block], HIGH);
  } 
}

void Board::output_directions(int frame[DIMENSION*DIMENSION]){
//  Magnet * mag_ptr;

//  //Shift register implementation
//  for(int block = 0; block < (DIMENSION*DIMENSION/4); block++){
//    digitalWrite(MSR_latchP, LOW);
//    shiftOut(MSR_dataP[0], MSR_clockP, LSBFIRST, ((frame[3] << 6) | (frame[2] << 4) | (frame[1] << 2) | frame[0]));
//    digitalWrite(MSR_latchP, HIGH);
//  }
}

void Board::set_zero(){
  float zero[DIMENSION*DIMENSION] = {0};
  output(zero);
}

void Board::output(float frame[DIMENSION*DIMENSION]){
  set_all_intensity(frame);
  output_directions();
}
