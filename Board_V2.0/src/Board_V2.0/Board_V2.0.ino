// Code developed by Ernesto Gomez
// netog@umich.edu
// BIM Lab - University of Michigan

#include "Magnet.h"
#include "Board.h"
#include "Communication.h"

const uint8_t connLED = 5;
const uint8_t clockP = 8;
const uint8_t latchP[NUM_ROWS*NUM_COLS] = {54, 55, 56, 57};
const uint8_t dataP[NUM_ROWS*NUM_COLS] = {10, 11, 12, 13};

Adafruit_PWMServoDriver PWM_Board = Adafruit_PWMServoDriver();
Board board(&PWM_Board, clockP, latchP, dataP);
Communication comm = Communication();
File myFile;
String incoming_command;

void setup() {
  Serial.begin(115200);
  PWM_Board.begin();
  PWM_Board.setPWMFreq(150);
  Wire.setClock(400000);
  comm.set(&Serial, &SD);
  pinMode(connLED, OUTPUT);
  board.set_zero();
}

void loop() {
  if(incoming_command == HALT){
    board.set_zero();
    grab_command();
    if (incoming_command == REQUEST){
      incoming_command = HALT;
    }
    else if(incoming_command == LIVE_ASYNC){
      live_async();
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
    else if(incoming_command == LOGIC){
      logic();
    }
  }
  else{
    if(!comm.establish_connection()){
      while(1){
        digitalWrite(connLED, (millis() % 5000 < 2500));
      }
    }
    incoming_command = HALT;
  }
}

void transfer(){
  digitalWrite(connLED, HIGH);
  
  comm.file_transfer();
  
  digitalWrite(connLED, LOW);
}

void fire(){
  digitalWrite(connLED, HIGH);
  uint8_t frame[NUM_ROWS*NUM_COLS];
  char str[7*NUM_ROWS*NUM_COLS];
  char r = NULL;
  int i = 0;
  int temp = 0;
  File file;
  
  file = SD.open(DEF_FILE, FILE_READ);
  if(file && file.peek() != -1){
    board.set_zero();
    while(file.peek() != -1){
      memset(frame, NULL, NUM_ROWS*NUM_COLS);
      memset(str, NULL, 7*NUM_ROWS*NUM_COLS);
      i = 0;
      r = file.read();
      do{
        str[i++] = r;
        r = file.read();       
        temp = (int)r;
      }while(temp != 10 && temp != 13 && temp != -1);    
      comm.string_to_frame(str, frame);
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
  uint8_t frame[NUM_ROWS*NUM_COLS];
  String incoming_msg;
  
  //to wait for frame, remove for speed
  comm.grab_command(incoming_msg, connLED);
  if(incoming_msg == HALT)return;
  
  for(int i = 0; i < NUM_ROWS*NUM_COLS; ++i){
    frame[i] = comm.grab_int(true, false, false, GRAB_TIMEOUT);
    Serial.println(frame[i]);
  }
  
  board.set_all_intensity(frame);
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

void live_async(){
  digitalWrite(connLED, HIGH);
  uint8_t frame[NUM_ROWS*NUM_COLS];
  uint8_t number = 0;
  bool flag = false;
  
  board.set_zero();
  while(number <= 100){
    for(uint8_t i = 0; i < NUM_ROWS*NUM_COLS; ++i){
      number = comm.grab_int(true, true, false, 1000);
      if (number > 100) {
        flag = true;
        break;
      };
      frame[i] = number;
    }
    if(flag) break;
    board.output(frame);
  }
  
  board.set_zero();
  if(number == 101)
    Serial.println(DONE);
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

void logic(){
  digitalWrite(connLED, HIGH);
  
  int frame[NUM_ROWS*NUM_COLS];
  int index = 0;
  
  board.set_zero();
  int number = comm.grab_int(false, false, false, GRAB_TIMEOUT);
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
    number = comm.grab_int(false, false, false, 1000);
  }
  
  board.set_zero();
  Serial.println(DONE);
  incoming_command = HALT;
  digitalWrite(connLED, LOW);
}

void grab_command(){
  incoming_command = "";
  comm.grab_command(incoming_command, connLED);
}

void set(Stream* Serial_ptr, SDLib::SDClass *SD_ptr, Adafruit_PWMServoDriver *PWM_Board_ptr){
  comm.set(Serial_ptr, SD_ptr);
  board.set(PWM_Board_ptr);
}