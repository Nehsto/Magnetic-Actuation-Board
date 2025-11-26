// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan
// Revision history: 
// 6/22/2024 - Notes introduced
// 4/1/2024 - Code used for UROP presentation

#include "Magnet.h"
#include "Board.h"
#include "Processing.h"

const unsigned int connLED = 5;
const unsigned int clockP = 8;
const unsigned int latchP[DIMENSION*DIMENSION] = {54, 55, 56, 57};
const unsigned int dataP[DIMENSION*DIMENSION] = {10, 11, 12, 13};
const unsigned int PWMP[DIMENSION*DIMENSION] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
const unsigned int upP[DIMENSION*DIMENSION] = {6,8,22,24,26,28,30,32,34,36,38,40,42,44,46,48};
const unsigned int downP[DIMENSION*DIMENSION] = {7,9,23,25,27,29,31,33,35,37,39,41,43,45,47,49};

Processing processor = Processing();
Board board(PWMP, upP, downP, clockP, latchP, dataP);
Adafruit_PWMServoDriver PWM_Board = Adafruit_PWMServoDriver();
File myFile;

String incoming_command;

void setup() {
  Serial.begin(9600);
  PWM_Board.begin();
  PWM_Board.setPWMFreq(1600);
  Wire.setClock(400000);
  set(&Serial, &SD, &PWM_Board);
  pinMode(connLED, OUTPUT);

  //DEBUG SECTION
  // pinMode(22, OUTPUT);
  // pinMode(23, OUTPUT);
  // pinMode(24, OUTPUT);
  // pinMode(25, OUTPUT);
  // pinMode(26, OUTPUT);
  // pinMode(27, OUTPUT);
  // pinMode(28, OUTPUT);
  // pinMode(29, OUTPUT);
  
  board.set_zero();
}

void loop() {
//  if(incoming_command == LIVE_SYNC){
//    live_sync();
//  }
//  else 
  if(incoming_command == HALT){//processor.is_connected()){
    board.set_zero();
    grab_command();
    if (incoming_command == REQUEST){
      incoming_command = HALT;
    }
    else if(incoming_command == LIVE_SYNC){
      live_sync();
    }
    else if(incoming_command == TRANSFER){
      transfer();
    }
    else if(incoming_command == FIRE){
      fire();
    }
    else if(incoming_command == LIVE_ASYNC){
      live_async();
    }
    else if(incoming_command == LOGIC){
      logic();
    }
  }
  else{
    if(!processor.establish_connection()){
      while(1){
        digitalWrite(connLED, (millis()%5000 < 2500));
      }
    }
    incoming_command = HALT;
  }
}

void transfer(){
  digitalWrite(connLED, HIGH);
  
  processor.file_transfer();
  
  digitalWrite(connLED, LOW);
}

void fire(){
  digitalWrite(connLED, HIGH);
  float frame[DIMENSION*DIMENSION];
  char str[7*DIMENSION*DIMENSION];
  char r = NULL;
  int i = 0;
  int temp = 0;
  File file;
  
  file = SD.open(DEF_FILE, FILE_READ);
  if(file && file.peek() != -1){
    board.set_zero();
    while(file.peek() != -1){
      memset(frame, NULL, DIMENSION*DIMENSION);
      memset(str, NULL, 7*DIMENSION*DIMENSION);
      i = 0;
      r = file.read();
      do{
        str[i++] = r;
        r = file.read();       
        temp = (int)r;
      }while(temp != 10 && temp != 13 && temp != -1);    
      processor.string_to_frame(str, frame);
      board.output(frame);
    }
    board.set_zero();
  }
  else{
    Serial.println(ERR_F);
  }
  
  Serial.println(DONE);
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

void live_sync(){
  digitalWrite(connLED, HIGH);
  float frame[DIMENSION*DIMENSION];
  String incoming_msg;
  
  //to wait for frame, remove for speed
  processor.grab_command(incoming_msg, connLED);
  if(incoming_msg == HALT)return;
  
  for(int i = 0; i < DIMENSION*DIMENSION; ++i){
    frame[i] = processor.grab_float(false, GRAB_TIMEOUT);
    Serial.println(int(frame[i]*100));
  }
  
  board.set_all_intensity(frame);
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

void live_async(){
  digitalWrite(connLED, HIGH);
  float frame[DIMENSION*DIMENSION];
  float number = 0;
  bool flag = false;
  
  board.set_zero();
  while(number <= 100.00){
    // This reads frames
    for(int i = 0; i < DIMENSION*DIMENSION; ++i){
      number = processor.grab_float(false, 1000);
      if (number > 100.00) {
        flag = true;
        break;
      };
      // The values are put together in the frame
      frame[i] = number;

      //DEBUG SECTION
      // for(int i = 0; i < DIMENSION*DIMENSION; ++i){
      //   Serial.print(board.board_layout[i].B_field);
      //   Serial.print(" ");
      // }
      // Serial.println();
    }

    //This outputs frames
    if(flag) break;
    board.output(frame);
  }
  
  board.set_zero();
  if(number == 100.01)Serial.println(DONE);
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

void logic(){
  digitalWrite(connLED, HIGH);
  
  int frame[DIMENSION*DIMENSION];
  int index = 0;
  
  board.set_zero();
  int number = processor.grab_int(false, false, false, GRAB_TIMEOUT);
  while(number < 255){
    index = 0;
    for(int i = 0; i < 8; ++i){
      if((number >> i) & 1){
        if(i%2)
          frame[index] = 2;
        else
          frame[index] = 1;
      }
      index += i%2;
    }
    board.output_directions(frame);
    number = processor.grab_int(false, false, false, 1000);
  }
  
  board.set_zero();
  Serial.println(DONE);
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

void grab_command(){
  incoming_command = "";
  processor.grab_command(incoming_command, connLED);
}

void set(Stream* Serial_ptr, SDLib::SDClass *SD_ptr, Adafruit_PWMServoDriver *PWM_Board_ptr){
  processor.set(Serial_ptr, SD_ptr);
  board.set(PWM_Board_ptr, Serial_ptr);
}

void TEST_LIVE_ASYNC_TIME(){
  digitalWrite(connLED, HIGH);
  float frame[DIMENSION*DIMENSION];
  float number = 0;
  bool flag = false;
  unsigned long avg_micros = 0, start_time = 0;
  
  board.set_zero();
  
  start_time = micros();
  while(number <= 100.00){
    for(int i = 0; i < DIMENSION*DIMENSION; ++i){
      number = processor.grab_float(false, 1000);
      if (number > 100.00) {
        flag = true;
        break;
      };
      frame[i] = number;
    }
    if(flag) break;
    board.output(frame);
  }
  avg_micros = (micros() - start_time);
  
  board.set_zero();
  if(number == 100.01)Serial.println(DONE);
  
  byte * b = (byte *) &avg_micros;
  Serial.write(b[0]);
  Serial.write(b[1]);
  Serial.write(b[2]);
  Serial.write(b[3]);
  
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

//void TEST_TIME_LOGIC(){
//  digitalWrite(connLED, HIGH);
//  
//  int frame[DIMENSION*DIMENSION];
//  int index = 0,
//  count = 0; 
//  
//  unsigned long avg_micros = 0, 
//  curr_time = 0;
//
//  curr_time = micros();
//  int number = processor.grab_int(false, false, false, GRAB_TIMEOUT);
//  
//  while(number < 255){
//    index = 0;
//    for(int i = 0; i < 8; ++i){
//      if((number >> i) & 1){
//        if(i%2)
//          frame[index] = 2;
//        else
//          frame[index] = 1;
//      }
//      index += i%2;
//    }
//    board.set_all_directions(frame);
//  
//    avg_micros += micros() - curr_time;
//    if (count >= 50){
//      avg_micros = avg_micros / 50;
//      Serial.println(avg_micros);
//      avg_micros = 0;
//      count = 0;
//    }
//    ++count;  
//    
//    curr_time = micros();
//    number = processor.grab_int(false, false, false, 5000);
//  }
//  
//  board.set_zero();
//  Serial.println(DONE);
//  incoming_command = HALT;
//  digitalWrite(connLED, LOW);
//}
