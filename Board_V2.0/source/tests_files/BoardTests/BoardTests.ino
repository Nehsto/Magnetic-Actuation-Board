#include "Magnet.h"
#include "Board.h"
#include "Processing.h"

const unsigned int connLED = 5;
const unsigned int clockP = 8;
const unsigned int latchP = 9;
const unsigned int dataP[DIMENSION*DIMENSION] = {10, 11, 12, 13};
const unsigned int PWMP[DIMENSION*DIMENSION] = {0,1,2,3};//,4,5,6,7,8,9,10,11,12,13,14,15};
const unsigned int upP[DIMENSION*DIMENSION] = {6,8,22,24};//,26,28,30,32,34,36,38,40,42,44,46,48};
const unsigned int downP[DIMENSION*DIMENSION] = {7,9,23,25};
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
  board.set_zero();
  digitalWrite(connLED, HIGH);
  delay(500);
  digitalWrite(connLED, LOW);
  delay(500);
}
void loop() {  
//  DEBUG_ARR();
  digitalWrite(connLED, HIGH);
  delay(500);
  if(!processor.establish_connection()){
    while(1){
      digitalWrite(connLED, (millis()%5000 < 2500));
    }
  }
  TEST_PWMV_OUTPUT();
}

//digitalWrite(connLED, HIGH);
//delay(500);
//digitalWrite(connLED, LOW);
//delay(500);
//TEST_PWMV_OUTPUT();

//void DEBUG_ARR(){
//  // Take a couple magnets, change their values and check the dir 
//  // and PWMv arrays to check that they are pointing to the correct
//  // address in memory.
//  
//  Magnet * m0 = board.get_magnet(0,0);
//  //Magnet * m1 = board.get_magnet(1,0);
//  char bufr[30];
//  sprintf(bufr, "%p", m0);
//
//  //First, confirm that the addresses are correct
//  Serial.println("Address values for magnet (0,0):");
//  Serial.println(bufr);
//  
//  Serial.println(&(m0->B_field));
//  Serial.println(board.dir[0]);
//  int *ptr_b = &(m0->B_field);
//  sprintf(bufr, "%p", ptr_b);
//  Serial.println(bufr);
//  sprintf(bufr, "%p", board.dir[0]);
//  Serial.println(bufr);
  
//  Serial.println("Values for magnet (1,0):");
//  Serial.println(m1 == board.board_layout[2]);
//  Serial.println(&(m1->voltage) == board.PWMv[2]);
//  Serial.println(&(m1->B_field) == board.dir[2]);

//  Change the voltage of one magnet and check that the voltage changed for m1 
//  Serial.println("Changing voltage to 5 V manually");
//  m0->set_voltage(5.0);
//  Serial.println(m0->voltage);
//  Serial.println(m0->B_field);
//  Serial.println(*(board.dir[0]));
//  Serial.println("Changing voltage to 5 V through board");
//  board.set_intensity(100, 0, 0);
//  Serial.println(m0->voltage);
//  Serial.println(m0->B_field);
//  Serial.println(*(board.dir[0]));
//  Serial.println("Changing voltage to -5 V through board");
//  board.set_intensity(-100, 0, 0);
//  Serial.println(m0->voltage);
//  Serial.println(m0->B_field); 
//  Serial.println(*(board.dir[0])); 
//  
//}

void TEST_PWMV_OUTPUT(){
  float number0[] = {100.0, 100.0, -100.0, -100.0};
  float number1[] = {-5.0, -5.0, -5.0, -5.0};
  float number2[] = {50.0, 0.0, 0.0, -50.0};
  float *numbers[] = {number0, number1, number2};
  board.output(number0);
//  Serial.println("Checking the B field of each magnet:");
//  for(int i = 0; i < 4; i++){
//    Serial.println(board.board_layout[i].B_field);
//  }
  delay(1000);
  board.output(number1);
//  Serial.println("Checking the B field of each magnet:");
//  for(int i = 0; i < 4; i++){
//    Serial.println(board.board_layout[i].B_field);
//  }
  delay(1000);
  board.output(number2);
//  Serial.println("Checking the B field of each magnet:");
//  for(int i = 0; i < 4; i++){
//    Serial.println(board.board_layout[i].B_field);
//  }
  delay(1000);

//  for(int i = 0; i < 3; ++i){
//    board.output(numbers[i]);
//    digitalWrite(5, LOW);
//    delay(1000);
//    digitalWrite(5, HIGH);
//  }
}

void TEST_READ_FLOAT(){
  float number = processor.grab_float(false, 1000);
  while(abs(number) < 101.0){
    Serial.println(int(number/100));
    number = processor.grab_float(false, 1000);
  }
}

//void TEST_set_voltage_intensity(){
//  for(float i = 0; i < 2*PI; i = i + 0.01){
//    myBoard.set_voltage(sin(i)/2, 0, 0);
//    myBoard.set_voltage(cos(i)/2, 0, 1);
//    myBoard.set_voltage(-cos(i)/2, 1, 0);
//    myBoard.set_voltage(-sin(i)/2, 1, 1);
//    myBoard.print_voltages();
//    delay(10);
//  }
//  for(float i = 0; i < 2*PI; i = i + 0.01){
//    myBoard.set_intensity(sin(i)*10, 0, 0);
//    myBoard.set_intensity(cos(i)*10, 0, 1);
//    myBoard.set_intensity(-cos(i)*10, 1, 0);
//    myBoard.set_intensity(-sin(i)*10, 1, 1);
//    myBoard.print_intensity();
//    delay(10);
//  }
//}
//void TEST_set_all(){
//  myBoard.set_all_intensity(frame1);
//  Serial.println(myBoard == frame1);
//  delay(500);
//  myBoard.set_all_intensity(frame2);
//  Serial.println(myBoard == frame2);
//  delay(500);
//  myBoard.set_all_intensity(frame3);
//  Serial.println(myBoard == frame3);
//  delay(500);
//  myBoard.set_zero();
//  Serial.println(myBoard == frame4);
//  delay(500);
//}

void set(Stream* Serial_ptr, SDLib::SDClass *SD_ptr, Adafruit_PWMServoDriver *PWM_Board_ptr){
  processor.set(Serial_ptr, SD_ptr);
  board.set(PWM_Board_ptr);
}
