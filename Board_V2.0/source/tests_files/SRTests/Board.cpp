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

Board::~Board(){
  for(int i = 0; i < DIMENSION*DIMENSION; i++){
    delete board_layout[i];
  }
}

Board::Board(int PWMPins[], int upPins[], int downPins[], int clockPin, int latchPin, int dataPin[DIMENSION*DIMENSION/4]){
  memset(board_layout, NULL, DIMENSION*DIMENSION);
  for(int i = 0; i < DIMENSION*DIMENSION; i++){
    board_layout[i] = new Magnet(PWMPins[i], upPins[i], downPins[i]);
  }
//  pinMode(clockPin, OUTPUT);
//  pinMode(latchPin, OUTPUT);
//  pinMode(dataPin, OUTPUT);
  
  MSR_clockP = clockPin;
  MSR_latchP = latchPin;
  for(int i = 0; i < DIMENSION*DIMENSION/4; ++i)
    MSR_dataP[i] = dataPin[i];
}

void Board::set(Adafruit_PWMServoDriver* Board_Ref){
  PWM_Board_ptr = Board_Ref;
}

Board::operator==(const float voltages[DIMENSION*DIMENSION]){
  for (int i = 0; i < DIMENSION*DIMENSION; i++){
    if (board_layout[i]->get_intensity() != voltages[i]){
      return false;
    }
  }
  return true;
}

Magnet * Board::get_magnet(int r, int c){
  return board_layout[(r * DIMENSION) + c];
}

void Board::set_voltage(float value, int r, int c){
  if(-5.0 > value || value > 5.0) return;
  Magnet * mag_ptr = get_magnet(r, c);
  mag_ptr -> Magnet::set_voltage(value, PWM_Board_ptr);
}

void Board::set_intensity(float value, int r, int c){
  if(-100.0 > value || value > 100.0) return;
  Magnet * mag_ptr = get_magnet(r, c);
  mag_ptr -> Magnet::set_intensity(value, PWM_Board_ptr);
}

void Board::set_all_voltages(float values[DIMENSION*DIMENSION]){
  for(int r = 0; r < DIMENSION; r++){
    for(int c = 0; c < DIMENSION; c++){
      set_voltage(values[DIMENSION*r+c], r, c);
    }
  }
}

void Board::set_all_intensity(float values[DIMENSION*DIMENSION]){
  for(int r = 0; r < DIMENSION; r++){
    for(int c = 0; c < DIMENSION; c++){
      set_intensity(values[DIMENSION*r+c], r, c);
    }
  }
}

void Board::set_all_directions(bool on_shift_reg){
  int block_byte;
  Magnet * mag_ptr;
  
  if(on_shift_reg){
    for(int block = 0; block < DIMENSION*DIMENSION/4; block++){
      block_byte = 0;
      for(int mag_id = 0; mag_id < 4; mag_id++){
        mag_ptr = get_magnet((block/(DIMENSION/4)), mag_id + (block%(DIMENSION/4)));
        block_byte = (block_byte << (mag_id*2)) || mag_ptr->get_B_field();
      } 
      //write the block byte to the shift register you want to control
      digitalWrite(MSR_latchP, LOW);
      shiftOut(MSR_dataP[block], MSR_clockP, MSBFIRST, block_byte);
      digitalWrite(MSR_latchP, HIGH);
    } 
  }
  else{
    for(int block = 0; block < (DIMENSION*DIMENSION/4); block++){
      for(int mag_id = 0; mag_id < 4; mag_id++){
        mag_ptr = get_magnet((block/4), mag_id + (block%4));
        digitalWrite(mag_ptr->get_upPin(), mag_ptr->get_B_field() & 1);
        digitalWrite(mag_ptr->get_downPin(), mag_ptr->get_B_field() >> 1);
      } 
    }
  }
}

void Board::set_all_directions(int frame[DIMENSION*DIMENSION]){
  Magnet * mag_ptr;
  
  for(int block = 0; block < (DIMENSION*DIMENSION/4); block++){
//    for(int mag_id = 0; mag_id < 4; mag_id++){
//      mag_ptr = get_magnet((block/4), mag_id + (block%4));
//      digitalWrite(mag_ptr->get_upPin(), frame[mag_id] & 1);
//      digitalWrite(mag_ptr->get_downPin(), (frame[mag_id] >> 1) & 1);
//    }
    digitalWrite(MSR_latchP, LOW);
    shiftOut(MSR_dataP[block], MSR_clockP, MSBFIRST, 0);
    digitalWrite(MSR_latchP, HIGH);
  }
}

void Board::set_zero(){
  for(int r = 0; r < DIMENSION; r++){
    for(int c = 0; c < DIMENSION; c++){
      set_voltage(0, r, c);
    }
  }
  set_all_directions(true);
  
}

void Board::run_frames_intensity(float* vals[DIMENSION*DIMENSION], const int frames, int delayms){ 
  set_zero();
  for(int i = 0; i < frames; i++){
    set_all_intensity(vals[i]);
    delay(delayms);
  }
  set_zero();
}

void Board::print_connections(){
  String str;
  Magnet *mag_ptr;
  for(int r = 0; r < DIMENSION; r++){
    for(int c = 0; c < DIMENSION; c++){
      mag_ptr = get_magnet(r,c);
      str = "{" + (String)mag_ptr->get_PWMPin() + ", " + (String)mag_ptr->get_upPin() + ", " + (String)mag_ptr->get_downPin() + "}";
      Serial.print(str);
    }
    Serial.println();
  }
}

int * Board::print_voltages(){
  String str = "";
  float temp = 0.0;
  int arr[DIMENSION*DIMENSION];
  memset(arr, 0, DIMENSION*DIMENSION); 
  Magnet *mag_ptr = NULL;
  Serial.print("{");
  for(int r = 0; r < DIMENSION; r++){
    for(int c = 0; c < DIMENSION; c++){
      mag_ptr = get_magnet(r,c);
      temp = mag_ptr->get_voltage();
      arr[r * DIMENSION + c] = temp;
      str = "[" + (String)temp + "]";
      Serial.print(str);
    }
    if (r != DIMENSION - 1){
      Serial.println();
    }
  }
  Serial.println("}");
  return arr;
}

int * Board::print_intensity(){
  String str = "";
  float temp = 0.0;
  int arr[DIMENSION*DIMENSION];
  memset(arr, 0, DIMENSION*DIMENSION); 
  Magnet *mag_ptr = NULL;
  Serial.print("{");
  for(int r = 0; r < DIMENSION; r++){
    for(int c = 0; c < DIMENSION; c++){
      mag_ptr = get_magnet(r,c);
      temp = mag_ptr->get_intensity();
      arr[r * DIMENSION + c] = temp;
      str = "[" + (String)temp + "]";
      Serial.print(str);
    }
    if (r != DIMENSION - 1){
      Serial.println();
    }
  }
  Serial.print("}");
  return arr;
}
