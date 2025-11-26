// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include "Magnet.h"
#include "Board.h"
#include "Communication.h"
#include "Arduino.h" 

Communication::Communication(){
  connected = false;
  memset(frame_array, NULL, NUM_ROWS*NUM_COLS);
  filepath = DEF_FILE;
};

void Communication::set(Stream *SerialRef, SDLib::SDClass *SDRef){
  Serial_ptr = SerialRef; 
  SD_ptr = SDRef;
}

void Communication::clear_file(){  
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

void Communication::println(char* txt){
  Serial_ptr->println(txt);
  flush();
};

void Communication::println(int txt){
  Serial_ptr->println(txt);
  flush();
};

void Communication::println(unsigned int txt){
  Serial_ptr->println(txt);
  flush();
};

void Communication::println(double txt){
  Serial_ptr->println(txt);
  flush();
};

void Communication::println(const char* txt){
  Serial_ptr->println(txt);
  flush();
};

void Communication::println(bool txt){
  Serial_ptr->println(txt);
  flush();
};
int Communication::available(){
  return Serial_ptr->available();
};

void Communication::flush(){
  Serial_ptr->flush();
};

int Communication::read() {
  Serial_ptr->read();
}

int Communication::peek() {
  Serial_ptr->peek();
}

int Communication::parseInt(){
  return Serial_ptr->parseInt(SKIP_NONE);
}

void Communication::clear(){
  while ((int)available() != 0){
    read();
  };
}

size_t Communication::readBytesUntil(char sen, char *msg, size_t len){
  return Serial_ptr->readBytesUntil(sen, msg, len);
};

void Communication::set_filepath(char* new_filepath){
  filepath = new_filepath;
};

bool Communication::acknowledge(const char* expected){
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

void Communication::grab_command(String &command, const uint8_t LED){
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
      strcmp(message_received, LIVE_SYNC) == 0 ||
      strcmp(message_received, LIVE_ASYNC) == 0 ||
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

void Communication::grab_message(String &msg, const bool print_ok, char sen, const uint32_t timeout){
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

float Communication::grab_float(const bool print_ok, const uint32_t timeout){
  if(!connected){
    println(ERR_C);
  }
  

  uint8_t byte0 = 0;
  uint8_t byte1 = 0;
  float received = 0.0;
  const int expected_len = 2;
  uint32_t start_time = millis();
  
  while(connected && millis() - start_time < timeout){
    //Waits until there is a message in the buffer
    if(available() >= expected_len){
      byte0 = read();
      byte1 = read();
      received = byte1 << 8 | byte0;
      //println(temp);
      //if the number of bytes received is equal to or greater than the size of the expected string
      if (-100.00 <= received && received <= 100.00){
        //if the message received is equal to the request string
        if(print_ok){
          println(OK);
        }
        return received;
        // If the message received is not equal to the expected string
      }
      else{
        println(ERR_L);
        return 102.0;
      }
    }
  }
  println(ERR_T);
  return 103;
};

int Communication::grab_int(const bool print_ok, const bool is_signed, const bool is_16bits, const uint32_t timeout){
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
    return 0xFFFF; 
  else 
    return 0XFF;
};

bool Communication::establish_connection(){
  connected = acknowledge(REQUEST);
  println(String(NUM_ROWS) + " + " + String(NUM_COLS));
  return connected;
};

uint8_t * Communication::parse_frame(){
  memset(frame_array, 0, sizeof(frame_array));
  for(uint8_t i = 0; i < NUM_ROWS * NUM_COLS; i++){
    frame_array[i] = grab_int(true, true, false, GRAB_TIMEOUT);
    if(frame_array[i] > 100){
      println(ERR_L);
      return;
    }
    else if(-100 > frame_array[i]){
      println(ERR_T);
      return;
    }
    if(i == (NUM_ROWS * NUM_COLS - 1)){
      println(FULL);
      break;
    }
    else{
      println(OK);
    }
  }
  return frame_array;
};

uint8_t * Communication::parse_frame_block(uint8_t ** frames, const uint8_t amt_blocks){
  uint8_t* total = malloc(NUM_ROWS * NUM_COLS * amt_blocks * sizeof(uint8_t)); // array to hold the result
  for(uint8_t i = 0; i < amt_blocks; i++){
    memcpy(total + i * NUM_ROWS * NUM_COLS, frames[i], NUM_ROWS * NUM_COLS * sizeof(uint8_t)); 
  }
  return total;
};

void Communication::parse_part_of_string(String &str, const uint8_t num_chunks){
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

void Communication::file_transfer(){
  uint8_t num_parts = 0;
  uint8_t num_chunks = 0;
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

void Communication::save_string(String &str){
  File frames_file = SD_ptr->open(filepath, FILE_WRITE);
  if(frames_file){
    frames_file.print(str);
    frames_file.close();
    println(SAVED);
  }else{
    println(ERR_F);
  }
};

void Communication::string_to_frame(char *str, uint8_t frame[NUM_ROWS*NUM_COLS]){
  char* token = strtok(str,",");
  int i = 0;
  while (token != NULL){
    frame[i] = atof(token);
    token = strtok(NULL, ",");
    i++; 
  }
};

bool Communication::is_connected(){
  return connected;
}
