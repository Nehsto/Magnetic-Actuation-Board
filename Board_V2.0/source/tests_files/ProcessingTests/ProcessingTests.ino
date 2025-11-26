#include <assert.h>
#include "Magnet.h"
#include "Board.h"
#include "Processing.h"

const int errLED = 5;
String incoming_command;

int PWMP[DIMENSION*DIMENSION] = {0,1,2,3};//,4,5,6,7,8,9,10,11,12,13,14,15};
int upP[DIMENSION*DIMENSION] = {6,8,22,24};//,26,28,30,32,34,36,38,40,42,44,46,48};
int downP[DIMENSION*DIMENSION] = {7,9,23,25};//,27,29,31,33,35,37,39,41,43,45,47,49};
Processing processor = Processing();
Board board(PWMP, upP, downP);
Adafruit_PWMServoDriver PWM_Board = Adafruit_PWMServoDriver();
File myFile;

void setup() {
  Serial.begin(9600);
  PWM_Board.begin();
  PWM_Board.setPWMFreq(1600);
  Wire.setClock(400000);
  set(&Serial, &SD, &PWM_Board);
  pinMode(errLED, OUTPUT); 
  
  board.set_zero();
}

void loop(){
  if(incoming_command == HALT){//processor.is_connected()){
    board.set_zero();
    grab_command();
    if(incoming_command == LOGIC){
      TEST_TIME();
    }
  }
  else{
    if(!processor.establish_connection()){
      while(1){
        digitalWrite(errLED, (millis()%5000 < 2500));
      }
    }
    incoming_command = HALT;
  }
}

void TEST_TIME(){
  digitalWrite(errLED, HIGH);
  
  int frame[DIMENSION*DIMENSION];
  int index = 0,
  count = 0, 
  avg_millis = 0, 
  curr_time = 0;

  curr_time = millis();
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
    board.set_all_directions(frame);
    
    avg_millis += millis() - curr_time;
    if (count >= 50){
      avg_millis = avg_millis / 50;
      Serial.write(avg_millis);
      avg_millis = 0;
    }
    ++count;  
    
    curr_time = millis();
    number = processor.grab_int(false, false, false, 1000);
  }
  
  board.set_zero();
  Serial.println(DONE);
  incoming_command = HALT;
  digitalWrite(errLED, LOW);
}

void grab_command(){
  incoming_command = "";
  processor.grab_command(incoming_command, errLED);
}

void fire(){
//  TEST_FIRE
  digitalWrite(errLED, HIGH);

  float frame[DIMENSION*DIMENSION];
  char str[7*DIMENSION*DIMENSION];
  char r = NULL;
  int i = 0;
  int temp = 0;
  File file;
  
  file = SD.open(DEF_FILE, FILE_READ);
  if(file){
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
      board.set_all_intensity(frame);
    }
    board.set_zero();
  }
  else{
    Serial.println(ERR_F);
  }
  
  digitalWrite(errLED, LOW);
}

void TEST_SET_ALL_INTENSITY(){
  digitalWrite(errLED, HIGH);
  delay(500);
  float frame1[DIMENSION*DIMENSION] = {100,100,100,100};
//  float frame2[DIMENSION*DIMENSION] = {75,75,75,75};
//  float frame3[DIMENSION*DIMENSION] = {-50,50,-50,50};
//  float frame4[DIMENSION*DIMENSION] = {1,-1,1,-1};
  board.set_zero();
  for(int i = 0; i < 100; ++i){
    for(int j = 0; j < 4; ++j){
      frame1[j] = i;
    }
    Serial.println(i);
    delay(10);
    board.set_all_intensity(frame1);
    delay(10);
  }
//  
//  board.set_zero();
//  delay(500);
//  board.set_all_intensity(frame2);
//  delay(500);
//  
//  board.set_zero();
//  delay(500);
//  board.set_all_intensity(frame3);
//  delay(500);
//  
//  board.set_zero();
//  delay(500);
//  board.set_all_intensity(frame4);
//  delay(500);
//  board.set_zero();
  
  digitalWrite(errLED, LOW);
  delay(500);
}
//  TEST_STRING_TO_FRAME
//  digitalWrite(errLED, LOW);
//  delay(500);
//
//  char* str = "100,0,50,50.1";
//  float frame[DIMENSION*DIMENSION];
//  processor.string_to_frame(str, frame);
//  for(int i = 0; i < DIMENSION*DIMENSION; i++){
//    Serial.println(frame[i]);
//  }
//  
//  digitalWrite(errLED, HIGH);
//  delay(500);
//}

void TEST_FILE_TRANSFER(){
//  TEST_FILE_TRANSFER
  digitalWrite(errLED, LOW);
  delay(500);
  
  processor.file_transfer();
  
  digitalWrite(errLED, HIGH);
  delay(500);
}

//  TEST_PARSE_STRING
//  digitalWrite(errLED, HIGH);
//  delay(500);
//
//  String str;
//  unsigned int num_chunks = 2;
//  
//  processor.parse_string(str, num_chunks);
//  Serial.println(str);
//
//  digitalWrite(errLED, LOW);
//  delay(500);

//  TEST_GRAB_MESSAGE
//  digitalWrite(errLED, HIGH);
//  delay(500);
//
//  String msg;
//  processor.grab_message(msg, true, '\r', GRAB_TIMEOUT);
//  Serial.println(msg);
//  
//  digitalWrite(errLED, LOW);
//  delay(500);

//  TEST_GRAB_UNSIGNED_INT
//  digitalWrite(errLED, HIGH);
//  delay(500);
//  
//  unsigned int temp = 0; 
//  temp = processor.grab_int(true, false, GRAB_TIMEOUT);
//  Serial.println(temp);
//  
//  digitalWrite(errLED, LOW);
//  delay(500);

//  TEST_GRAB_INT
//  digitalWrite(errLED, HIGH);
//  delay(500);
//  
//  unsigned int temp = 0; 
//  temp = processor.grab_int(true, true, GRAB_TIMEOUT);
//  Serial.println(temp);
//  
//  digitalWrite(errLED, LOW);
//  delay(500);

//  TEST_PARSE_STRING
//  digitalWrite(errLED, HIGH);
//  delay(500);
//  
//  num_parts = 0;
//  num_chunks = 0;
//  str = "";
//  myFile = SD.open("test.txt");
//  if(!myFile){
//    while(1);
//  }
//  num_parts = processor.grab_int(6, GRAB_TIMEOUT, false);
//  for(int part = 0; part < num_parts; part++){
//    num_chunks = processor.grab_int(4, GRAB_TIMEOUT, false);
//    for(int chunk = 0; chunk < num_chunks; chunk++){
//      str = processor.parse_string(num_chunks);
//      myFile.print(str);
//    }
//  }
//  myFile.close();
//  digitalWrite(errLED, LOW);
//  delay(500);

//  TEST_SAVE_FRAME
//  digitalWrite(errLED, HIGH);
//  delay(500);
//  float frame[] = {0, 1, 50, 100};
//  processor.save_frame(frame);
//  digitalWrite(errLED, LOW);
//  delay(500);

//  TEST_SAVE_FRAME_BROKENDOWN
//  digitalWrite(errLED, HIGH);
//  delay(500);
//  float frame[] = {0, 1, 50, 100};
//  char str[STREAM_BUFFER_SIZE];
//  char block[BLOCK_BUFFER_SIZE];
//  File frames_file = SD.open(DEF_FILEPATH, FILE_WRITE);
//  if(frames_file){
//    for(int i = 0; i < DIMENSION*DIMENSION; i++){
//      dtostrf(frame[i], FLOAT_PRECISION+2, FLOAT_PRECISION, str);
//      Serial.println(frame[i]);
//      Serial.println(str);
//      if(i != (DIMENSION*DIMENSION - 1)){
//        strcat(str, ", ");
//      }      
//      strcat(row, str);
//    }
//    frames_file.println(row);
//    frames_file.close();
//    Serial.println(SAVED);
//  }else{
//    Serial.println(ERR_F);
//  }
//  digitalWrite(errLED, LOW);
//  delay(500);

void set(Stream* Serial_ptr, SDLib::SDClass *SD_ptr, Adafruit_PWMServoDriver *PWM_Board_ptr){
  processor.set(Serial_ptr, SD_ptr);
  board.set(PWM_Board_ptr);
}
