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

Board::Board(int PWMPins[], int upPins[], int downPins[], int clockPin, int latchPin, int dataPin[]){
  memset(board_layout, NULL, DIMENSION*DIMENSION);
  for(int i = 0; i < DIMENSION*DIMENSION; i++){
    board_layout[i] = Magnet(PWMPins[i], upPins[i], downPins[i]);
  }

  MSR_clockP = clockPin;
  MSR_latchP = latchPin;

  pinMode(MSR_clockP, OUTPUT);
  pinMode(MSR_latchP, OUTPUT);
  for(int i = 0; i < 4; ++i){ //TODO: CHANGE THE 4 TO A NUMBER RELATIVE TO THE DIMENSION
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

//void Board::set_voltage(float value, int r, int c){
//  if(-5.0 > value || value > 5.0) return;
//  get_magnet(r, c)-> set_voltage(value);
//}

void Board::set_intensity(float value, int r, int c){
  Magnet* mag_ptr = get_magnet(r, c); 
  mag_ptr -> Magnet::set_intensity(value);
  PWM_Board_ptr->setPWM((r*DIMENSION)+c, 0, uint16_t((abs(mag_ptr->voltage)/PIN_VOLTAGE)*PWM_RATIO)-1); 
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

void Board::output_directions(bool on_shift_reg){
  unsigned int block_byte;
  Magnet* mag_ptr;
  
  if(on_shift_reg){
    for(int block = 0; block < DIMENSION*DIMENSION/4; block++){
      block_byte = 0;
      for(int mag_id = 0; mag_id < 4; mag_id++){
        //TODO: Figure out what to do with block and mag_id when board changes to 8x8
        block_byte = (block_byte << 2) | (board_layout[mag_id].B_field & 3);        
        Serial.println(block_byte, BIN);
      } 
      Serial.println(block_byte, BIN);
      //write the block byte to the shift register you want to control
      digitalWrite(MSR_latchP, LOW);
      shiftOut(MSR_dataP[block], MSR_clockP, MSBFIRST, block_byte);
      digitalWrite(MSR_latchP, HIGH);
    } 
  }
//  else{
//    for(int block = 0; block < (DIMENSION*DIMENSION/4); block++){
//      for(int mag_id = 0; mag_id < 4; mag_id++){
//        mag_ptr = get_magnet((block/4), mag_id + (block%4));
//        digitalWrite(mag_ptr->upPin, mag_ptr->B_field & 1);
//        digitalWrite(mag_ptr->downPin, mag_ptr->B_field >> 1);
//      } 
//    }
//  }
}

// void Board::output_directions(int frame[DIMENSION*DIMENSION]){
  // Magnet * mag_ptr;

  // Shift register implementation
  // for(int block = 0; block < (DIMENSION*DIMENSION/4); block++){
  // digitalWrite(MSR_latchP, LOW);
  // shiftOut(MSR_dataP[0], MSR_clockP, MSBFIRST, ((frame[3] << 6) | (frame[2] << 4) | (frame[1] << 2) | frame[0]));
  // digitalWrite(MSR_latchP, HIGH);
  // }

//  Directly to digital pins
//  for(int block = 0; block < (DIMENSION*DIMENSION/4); block++){
//    for(int mag_id = 0; mag_id < 4; mag_id++){
//      mag_ptr = get_magnet((block/4), mag_id + (block%4));
//      digitalWrite(mag_ptr->get_upPin(), frame[mag_id] & 1);
//      digitalWrite(mag_ptr->get_downPin(), (frame[mag_id] >> 1) & 1);
//    }
//  }
//}

void Board::set_zero(){
  float zero[DIMENSION*DIMENSION] = {0};
  output(zero);
}

void Board::output(float frame[DIMENSION*DIMENSION]){
  set_all_intensity(frame);
  output_directions(true);
}
