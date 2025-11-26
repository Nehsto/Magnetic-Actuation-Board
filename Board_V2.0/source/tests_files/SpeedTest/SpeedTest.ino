// Code developed by Ernesto Gomez
// netog@umich.edu
// BIM Lab - University of Michigan

#include "Magnet.h"
#include "Board.h"
#include "Processing.h"

const unsigned int connLED = 5;
const unsigned int NUMPIN = 7;
const unsigned int FRAMEPIN = 9;
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
  Serial.begin(115200);
  PWM_Board.begin();
  PWM_Board.setPWMFreq(150);
  Wire.setClock(400000);
  set(&Serial, &SD, &PWM_Board);
  pinMode(connLED, OUTPUT);
  pinMode(NUMPIN, OUTPUT);
  pinMode(FRAMEPIN, OUTPUT);

  digitalWrite(NUMPIN, LOW);
  digitalWrite(FRAMEPIN, LOW);

  //DEBUG SECTION
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
  int frame[DIMENSION*DIMENSION];
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
//      processor.string_to_frame(str, frame);
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
  int frame[DIMENSION*DIMENSION];
  String incoming_msg;
  
  //to wait for frame, remove for speed
  processor.grab_command(incoming_msg, connLED);
  if(incoming_msg == HALT)return;
  
  for(int i = 0; i < DIMENSION*DIMENSION; ++i){
    frame[i] = processor.grab_float(false, GRAB_TIMEOUT);
  }
  
  board.set_all_intensity(frame);
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

void live_async(){
  digitalWrite(connLED, HIGH);
  int frame[DIMENSION*DIMENSION];
  int number = 0;
  bool flag = false;
  
  board.set_zero();
  while(number <= 100){
    // This reads frames
    //digitalWrite(FRAMEPIN, HIGH);
    for(int i = 0; i < DIMENSION*DIMENSION; ++i){
      //digitalWrite(NUMPIN, HIGH);
      number = processor.grab_int(true, true, false, GRAB_TIMEOUT);
      if (number > 100) {
        flag = true;
        break;
      };
      // The values are put together in the frame
      frame[i] = number;
      //digitalWrite(NUMPIN, LOW);
    }

    //This outputs frames
    if(flag) break;
    board.output(frame);
    //digitalWrite(FRAMEPIN, LOW);
  }
  //digitalWrite(NUMPIN, LOW)
  //digitalWrite(FRAMEPIN, LOW)
  
  board.set_zero();
  if(number == 101)Serial.println(DONE);
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
    board.output_directions();
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
  board.set(PWM_Board_ptr);
}
