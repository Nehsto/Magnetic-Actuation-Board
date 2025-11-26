#define __ASSERT_USE_STDERR
#include <assert.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "Magnet.h"
#include "Board.h"
#include "Processing.h"
#include "Arduino.h" 

Processing::Processing(){
  connected = false;
  memset(frame_array, NULL, DIMENSION*DIMENSION);
  filepath = DEF_FILE;
};

Processing::~Processing(){

};

void Processing::set(Stream *SerialRef, SDLib::SDClass *SDRef){
  Serial_ptr = SerialRef; 
  SD_ptr = SDRef;
  
  if(!SD_ptr->begin(CSPin)) {
    println(ERR_C);
    while(1);
  }
}

void Processing::clear_file(){  
  if(SD_ptr->exists(DEF_FILE)){
    if(!SD_ptr->remove(DEF_FILE)){
      println(ERR_F);
      return;
    }
  }
  
  File frames_file = SD_ptr->open(DEF_FILE, FILE_WRITE);
  if(frames_file){
    frames_file.close();
  }
  else{
    println(ERR_F);
  }
};

void Processing::println(char* txt){
  Serial_ptr->println(txt);
  flush();
};

void Processing::println(int txt){
  Serial_ptr->println(txt);
  flush();
};

void Processing::println(unsigned int txt){
  Serial_ptr->println(txt);
  flush();
};

void Processing::println(double txt){
  Serial_ptr->println(txt);
  flush();
};

void Processing::println(const char* txt){
  Serial_ptr->println(txt);
  flush();
};

void Processing::println(bool txt){
  Serial_ptr->println(txt);
  flush();
};
int Processing::available(){
  return Serial_ptr->available();
};

void Processing::flush(){
  Serial_ptr->flush();
};

int Processing::read() {
  Serial_ptr->read();
}

int Processing::peek() {
  Serial_ptr->peek();
}

int Processing::parseInt(){
  return Serial_ptr->parseInt(SKIP_NONE);
}

void Processing::clear(){
  while ((int)available() != 0){
    read();
  };
}

size_t Processing::readBytesUntil(char sen, char *msg, int len){
  return Serial_ptr->readBytesUntil(sen, msg, len);
};

void Processing::set_filepath(char* new_filepath){
  filepath = new_filepath;
};

bool Processing::acknowledge(const char* expected){
  char message_received[strlen(expected)+1] = {"\0"};
  size_t bytes_received;

  clear();
  while(true){
    //Waits until there is a message in the buffer
    if(available() != 0){
      bytes_received = readBytesUntil('\r', message_received, strlen(expected));
      //if the number of bytes received is equal to or greater than the size of the expected string
      //(for some reason bytes_received only goes to 5, but hey it works)
      if (bytes_received == strlen(expected)){
        //if the message received is equal to the request string
        if (strcmp(message_received, expected) == 0){
          println(ACKNOWLEDGE);
          clear();
          return true;
        }
        // if the message received is not equal to the expected string
      }
      clear();
    }
  }
};

void Processing::grab_command(String &command, const unsigned int LED){
  if(!connected){
    println(ERR_C);
  }
    
  char message_received[BUFFER_SIZE] = {"\0"};
  size_t bytes_received;
  
  while(connected){
    digitalWrite(LED, (millis()%1000 < 500));
    //Waits until there is a message in the buffer
    if(available() != 0){
      bytes_received = readBytesUntil('\r', message_received, BUFFER_SIZE);
      // if the message received is a high level command, acknowledge
      if(strcmp(message_received, REQUEST) == 0 ||
      strcmp(message_received, TRANSFER) == 0 ||
      strcmp(message_received, FIRE) == 0 ||
      strcmp(message_received, LIVE) == 0 ||
      strcmp(message_received, LOGIC) == 0){
        println(ACKNOWLEDGE);  
        command = message_received;
        return;
      }
      // else, return command error
      else{
        Serial.println(ERR_A);
        command = HALT;
        return;
      }
    }
  }
  println(ERR_C);
  return;
};

void Processing::grab_message(String &msg, const bool print_ok, char sen, const unsigned long timeout){
  if(!connected){
    println(ERR_C);
  }
    
  char message_received[BUFFER_SIZE] = {'\0'};
  size_t bytes_received;
  unsigned long start_time = millis();
  
  while(connected && millis() - start_time < timeout){
    // Waits until there is a message in the buffer.
    // If the number of bytes received is equal to 
    // or greater than the size of the expected string
    if(available() != 0){
      bytes_received = readBytesUntil(sen, message_received, BUFFER_SIZE);
      // if the message received is equal to the request string
      if(print_ok){
        println(OK);  
      }
      msg = message_received;
      return;
      // if the message received is not equal to the expected string
    }
  }
  println(ERR_T);
  return;
};

float Processing::grab_float(const bool print_ok, const unsigned long timeout){
  if(!connected){
    println(ERR_C);
  }
  
  int temp = 0; 
  int byte0 = 0;
  int byte1 = 0;
  float received = 0.0;
  const int expected_len = 2;
  unsigned long start_time = millis();
  
  while(connected && millis() - start_time < timeout){
    //Waits until there is a message in the buffer
    if(available() >= expected_len){
      byte0 = read();
      byte1 = read();
      temp = byte1 << 8 | byte0;
      //if the number of bytes received is equal to or greater than the size of the expected string
      if (-10000 <= temp && temp <= 10000){
        //if the message received is equal to the request string
        if(print_ok){
          println(OK);
        }
        received = ((float)temp)/100;
        return received;
        // If the message received is not equal to the expected string
      }
      else{
        println(ERR_L);
        return 101;
      }
    }
  }
  println(ERR_T);
  return -101;
};

int Processing::grab_int(const bool print_ok, const bool is_signed, const bool is_16bits, const unsigned long timeout){
  if(!connected){
    println(ERR_C);
  }
  int received = 0;
  unsigned int byte0 = 0;
  unsigned int byte1 = 0;
  const int expected_len = 1+is_16bits;
  unsigned long start_time = millis();

  while(connected && millis() - start_time < timeout){
    //Waits until there is a message in the buffer
    if(available() >= expected_len){
      byte0 = read();
      if (is_16bits){
        byte1 = read();
        received = byte1 << 8 | byte0;
      }
      else
        received = byte0;
      
      if(is_signed){
        received = (signed int)received;
      }
      if(print_ok){
        println(OK);
      }
      
      clear();
      return received;
    }
  }
  println(ERR_T);
  
  if(is_16bits) 
    return pow(2,16)-1; 
  else 
    return pow(2,8)-1;
};

bool Processing::establish_connection(){
  connected = acknowledge(REQUEST);
  println(DIMENSION);
  return connected;
};

float * Processing::parse_frame(){
  memset(frame_array, 0, sizeof(frame_array));
  for(int i = 0; i < DIMENSION*DIMENSION; i++){
    frame_array[i] = grab_float(GRAB_TIMEOUT, false);
    if(frame_array[i] > 100){
      println(ERR_L);
      return;
    }
    else if(-100 > frame_array[i]){
      println(ERR_T);
      return;
    }
    if(i == (DIMENSION*DIMENSION - 1)){
      println(FULL);
      break;
    }
    else{
      println(OK);
    }
  }
  return frame_array;
};

float * Processing::parse_frame_block(float ** frames, const unsigned int amt_blocks){
  float* total = malloc(DIMENSION * DIMENSION * amt_blocks * sizeof(float)); // array to hold the result
  for(int i = 0; i < amt_blocks; i++){
    memcpy(total + i * DIMENSION * DIMENSION, frames[i], DIMENSION * DIMENSION * sizeof(float)); 
  }
  return total;
};

void Processing::save_frame_block(char* str){
//  char str[STREAM_BUFFER_SIZE] = {"\0"};
//  char block[BLOCK_BUFFER_SIZE] = {"\0"};
//  File frames_file = SD_ptr->open(filepath, FILE_WRITE);
//  if(frames_file){
//    for(int frame = 0; i < BLOCK_SIZE/(DIMENSION*DIMENSION); i++){
//      if(amt_frames){
//        
//      }
//      for(int i = 0; i < DIMENSION*DIMENSION; i++){
//        dtostrf(frame[i], FLOAT_PRECISION+2, FLOAT_PRECISION, str);
//        if(i != (DIMENSION*DIMENSION - 1)){
//          strcat(str, ", ");
//        }      
//        strcat(row, str);
//      }
//      strcat(row, "\n");
//    }
//    frames_file.println(row);
//    frames_file.close();
//    println(SAVED);
//  }else{
//    println(ERR_F);
//  }
};

void Processing::parse_part_of_string(String &str, const unsigned int num_chunks){
  String msg;
  for(int i = 0; i < num_chunks; i++){
    grab_message(msg, false, '\r', GRAB_TIMEOUT);
    str = str + msg;
    if (i >= (num_chunks - 1)){
      println(FULL);
      return;
    }
    else{
      println(OK);
    }   
  }
  return;
};

void Processing::file_transfer(){
  unsigned int num_parts = 0;
  unsigned int num_chunks = 0;
  String str = "";
  clear();
  clear_file();

  //function changed, might need debugging
  num_parts = grab_int(true, false, true, GRAB_TIMEOUT);
  for(int part = 0; part < num_parts; part++){
    str = "";
    num_chunks = grab_int(true, false, true, GRAB_TIMEOUT);
    parse_part_of_string(str, num_chunks);
    save_string(str);
  }
  println(DONE);
};

void Processing::save_string(String &str){
  File frames_file = SD_ptr->open(filepath, FILE_WRITE);
  if(frames_file){
    frames_file.print(str);
    frames_file.close();
    println(SAVED);
  }else{
    println(ERR_F);
  }
};

void Processing::string_to_frame(char *str, float frame[DIMENSION*DIMENSION]){
  char* token = strtok(str,",");
  int i = 0;
  while (token != NULL){
    frame[i] = atof(token);
    token = strtok(NULL, ",");
    i++; 
  }
};

bool Processing::is_connected(){
  return connected;
}
