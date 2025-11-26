// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#ifndef Communication_h
#define Communication_h
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "Arduino.h" 
#include "Board.h"

// Notes for command nomenclature
// TYPE OF COMMAND(READ, WRITE, READ/WRITE): description of command

// HIGH LEVEL COMMANDS: for entering a processing mode
// READ: transfer frames to run
#define TRANSFER 'T'
// READ: run transferred data
#define FIRE 'F'
// READ: run live synchronous mode on board
#define LIVE_SYNC 'L'
// READ: run live asynchronous mode on board
#define LIVE_ASYNC 'A'
// READ: run binary mode on board
#define LOGIC 'B'
// READ: MATLAB commands to stop operation and go to menu
#define HALT 'H'

// LOW LEVEL COMMANDS: for data parsing purposes
// READ/WRITE: Request to do something. 
#define REQUEST 'R'
// READ/WRITE: Acknowledgment used when receiving a high level command
#define ACKNOWLEDGE 'C'
// WRITE: Obtained a data chunk successfully
#define OK 'K'
// WRITE: Obtained all the data for a frame
#define FULL 'F'
// WRITE: Obtained all the data necessary to run signal
#define DONE 'D'
// WRITE: File was saved successfully
#define SAVED 'S'

// ERROR CODES
// READ/WRITE: Raise error. Might need a couple codes
// (C) - Code letter
// ERR_A: Command Error
#define ERR_A "EA"
// ERR_T: Timed out
#define ERR_T "ET"
// ERR_L: Length of command was shorter than expected
#define ERR_L "EL"
// ERR_C: Board is not connected
#define ERR_C "EC"
// ERR_C: File error - SD Card
#define ERR_F "EF"

// DEFAULT FILE DATA
// DEF_FILE: Default file
#define DEF_FILE "def.txt"

const uint16_t GRAB_TIMEOUT = 2000;
const uint8_t BUFFER_SIZE = 64;
const uint8_t MSG_LEN = 6;

const uint8_t BLOCK_SIZE = 64;
const uint8_t MISOPin = 50;
const uint8_t MOSIPin = 51;
const uint8_t CLKPin = 52;
const uint8_t CSPin = 53;

class Communication{
  private:
    // Pointer to store reference to Serial object in main sketch
    Stream *Serial_ptr;
    // Pointer to store reference to SD object in main sketch
    SDLib::SDClass * SD_ptr;
    
    // Stores if board is serial port is connected.
    bool connected;
    
    // Array of values.
    uint8_t frame_array[NUM_COLS*NUM_ROWS];
    
    // Default file to store latest coming array of frames. 
    // Will be stored in SD card as "def.txt"
    char * filepath;

    void println(char* txt);// Equivalent to Serial.println()
    void println(int txt);
    void println(unsigned int txt);
    void println(double txt);
    void println(const char* txt);
    void println(bool txt);    
    int available();// Equivalent to Serial.available()    
    void flush();// Equivalent to Serial.flush()   
    int read();// Equivalent to Serial.read()    
    int peek();// Equivalent to Serial.peek()
    int parseInt();// Equivalent to Serial.parseInt()    
    void clear();// Equivalent to Serial.clear()
    // Equivalent to Serial.readBytesUntil()
    size_t readBytesUntil(char sen, char *msg, size_t len);
    
    // Used in file_transfer() for parsing sent string
    void parse_part_of_string(String &str, const unsigned int num_chunks);

    // Used in file_transfer() for saving string
    void save_string(String &str);
    
  public:
    Communication();
    
    // Pass the reference to the Serial object in main sketch
    void set(Stream* SerialRef, SDLib::SDClass* SDRef);
    
    // Sends request.
    void request();

    // Waits for request indefinitely, sends acknowledgement if
    // obtained successfully. 
    bool acknowledge(const char* expected);
    
    // Waits for high-level commands indefinitely. Sends 
    // acknowledgement if obtained successfully. 
    // HIGH-LEVEL COMMANDS ACCEPTED:
    //  - REQUEST
    //  - TRANSFER
    //  - FIRE
    //  - LIVE_SYNC
    //  - LIVE_ASYNC
    //  - LOGIC
    void grab_command(String &command, const uint8_t LED);
    
    // Waits for message with anu size, sends ok signal
    // if grabbed successfully. Times out after a certain period
    void grab_message(String &msg, const bool print_ok, char sen, const uint32_t timeout);

    // Waits for signed float, sends OK if print_ok
    float grab_float(const bool print_ok, const uint32_t timeout);

    // Waits for signed int16(can be changed with bools. If not 16bits, it's 8bit), sends OK if print_ok
    int grab_int(const bool print_ok, const bool is_signed, const bool is_16bits, const uint32_t timeout);

    // Acknowledge, then wait for and confirm that: 
    // - The dimension of the board is equal to the dimension sent.
    // - TODO: Amount of frames that will be sent
    bool establish_connection();

    // Delete file and create a blank one
    void clear_file();

    // Change filepath (private object in Processing class)
    void set_filepath(char* new_filepath);

    // Parse frame int by int. Send FULL when frame is processed. 
    uint8_t *parse_frame();

    uint8_t *parse_frame_block(uint8_t ** frames, const uint8_t amt_blocks);

    void parse_part_of_string(String &str, const uint8_t num_chunks);

    void file_transfer();

    // For now, assume values are stored in intensity
    void string_to_frame(char *str, uint8_t frame[NUM_COLS*NUM_ROWS]);

    bool is_connected();
};

#endif
