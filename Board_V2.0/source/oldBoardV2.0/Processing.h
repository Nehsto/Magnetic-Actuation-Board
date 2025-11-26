// Code developed by Ernesto Gomez
// BIM Lab - University of Michigan

#ifndef Processing_h
#define Processing_h
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "Arduino.h" 
#include "Board.h"

// HIGH LEVEL COMMANDS
// READ: MATLAB commands to transfer frames to run
#define TRANSFER "TRFR"
// READ: MATLAB commands to run transferred data
#define FIRE "FIRE"
// READ: MATLAB commands to run live (synchronous) mode on board
#define LIVE_SYNC "LIVE"
// READ: MATLAB commands to run live asynchronous mode on board
#define LIVE_ASYNC "LV_A"
// READ: MATLAB commands to run live mode on board
#define LOGIC "LGIC"
// READ: MATLAB commands to stop operation and go to menu
#define HALT "HALT"

// LOW LEVEL COMMANDS
// READ/WRITE: Request to do something. 
#define REQUEST "READY"
// READ/WRITE: Acknowledgment used when receiving a high level command
#define ACKNOWLEDGE "ACK"
// WRITE: Obtained a data chunk successfully
#define OK "OK"
// WRITE: Obtained all the data for a frame
#define FULL "FULL"
// WRITE: Obtained all the data necessary to run signal
#define DONE "DONE"
// WRITE: File was saved successfully
#define SAVED "SAVED"

// ERROR CODES
// READ/WRITE: Raise error. Might need a couple codes
// (C) - Code letter
// ERR_A: Command Error
#define ERR_A "ERR_A"
// ERR_T: Timed out
#define ERR_T "ERR_T"
// ERR_L: Length of command was shorter than expected
#define ERR_L "ERR_L"
// ERR_C: Board is not connected
#define ERR_C "ERR_C"
// ERR_C: File error
#define ERR_F "ERR_F"

// DEFAULT FILE DATA
// DEF_FILE: Default file
#define DEF_FILE "def.txt"

const unsigned long GRAB_TIMEOUT = 1000;
const unsigned int BUFFER_SIZE = 64;
const unsigned int MSG_LEN = 6;
const unsigned int STR_LEN = 1024;

const unsigned int FLOAT_PRECISION = 2;
const unsigned int BLOCK_SIZE = 64;
const unsigned int STREAM_BUFFER_SIZE = FLOAT_PRECISION+5;
const unsigned int BLOCK_BUFFER_SIZE = (FLOAT_PRECISION+7)*BLOCK_SIZE;

const unsigned int MISOPin = 50;
const unsigned int MOSIPin = 51;
const unsigned int CLKPin = 52;
const unsigned int CSPin = 53;

class Processing{
  private:
    // Pointer to store reference to Serial object in main sketch
    Stream *Serial_ptr;
    // Pointer to store reference to SD object in main sketch
    SDLib::SDClass * SD_ptr;
    
    // Stores if board is connected to MATLAB or not.
    bool connected;
    
    // Array of values. NOT IN USE AS OF NOW. MIGHT DELETE LATER
    float frame_array[DIMENSION*DIMENSION];
    
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
    size_t readBytesUntil(char sen, char *msg, int len);
    
    // Used in file_transfer() for parsing sent string
    void parse_part_of_string(String &str, const unsigned int num_chunks);

    // Used in file_transfer() for saving string
    void save_string(String &str);
    
  public:
    Processing();
    
    ~Processing();
    
    // Pass the reference to the Serial object in main sketch
    void set(Stream * SerialRef, SDLib::SDClass * SDRef);
    
    // Delete file and create a blank one
    void clear_file();

    // Change filepath (private object in Processing class)
    void set_filepath(char* new_filepath);
    
    // Sends request. Placeholder function, as no request 
    // command is needed so far.
    void request();

    // Waits for request indefinitely, sends acknowledgement if
    // obtained successfully. 
    bool acknowledge(const char* expected);
    
    // Waits for high-level commands indefinitely. Sends 
    // acknowledgement if obtained successfully. 
    // HIGH-LEVEL COMMANDS ACCEPTED:
    //  - TRANSFER
    //  - FIRE
    //  - LIVE_SYNC
    //  - LIVE_ASYNC
    //  - LOGIC
    void grab_command(String &command, const unsigned int LED);
    
    // Waits for message with anu size, sends ok signal
    // if grabbed successfully. Times out after a certain period
    void grab_message(String &msg, const bool print_ok, char sen, const unsigned long timeout);

    // Waits for signed float, sends OK if print_ok
    // Returns floats in range [-100, 103]
    // Returning 100.01 means stop reading 
    // Returning 102 means byte is out of the range to be read
    // Returning 103 means timeout
    float grab_float(const bool print_ok, const unsigned long timeout);

    // Waits for signed int16(can be changed with bools. If not 16bits, it's 8bit), sends OK if print_ok
    int grab_int(const bool print_ok, const bool is_signed, const bool is_16bits, const unsigned long timeout);

    // Acknowledge, then wait for and confirm that: 
    // - The dimension of the board is equal to the dimension sent.
    // - TODO: Amount of frames that will be sent
    bool establish_connection();

    // Parse frame int by int. Send FULL when frame is processed. 
    float * parse_frame();

    // NOT PROVEN TO WORK
    float * parse_frame_block(float ** frames, const unsigned int amt_blocks);

    // Stores frame in file. NOT PROVEN TO WORK
    void save_frame_block(char * str);

    void file_transfer();

    // For now, assume values are stored in intensity
    void string_to_frame(char *str, float frame[DIMENSION*DIMENSION]);

    bool is_connected();
};

#endif
