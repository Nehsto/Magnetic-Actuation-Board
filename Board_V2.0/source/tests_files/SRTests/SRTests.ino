const unsigned int clockP = 8;
const unsigned int latchP = 9;
const unsigned int dataP[4] = {10, 11, 12, 13};

void setup(){
  Serial.begin(9600);
  pinMode(latchP, OUTPUT);
  pinMode(clockP, OUTPUT);
  for(int i = 0; i < 4; i++){
    pinMode(dataP[i], OUTPUT);
  }

//  for(int block = 0; block < 4; block++){
//    digitalWrite(latchP, LOW);
//    shiftOut(dataP[block], clockP, MSBFIRST, 0);
//    digitalWrite(latchP, HIGH);
//  }
}

void loop(){
  int x;
  for(int block = 0; block < 4; block++){
    for(int i = 0; i < 8; i++){
      x = 1 << i;
      digitalWrite(latchP, LOW);
      shiftOut(dataP[block], clockP, MSBFIRST, x);
      digitalWrite(latchP, HIGH);
      delay(100);
    } 
    digitalWrite(latchP, LOW);
    shiftOut(dataP[block], clockP, MSBFIRST, 0);
    digitalWrite(latchP, HIGH);
  }
  
}
