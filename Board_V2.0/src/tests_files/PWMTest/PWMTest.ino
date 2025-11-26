uint8_t i = 0;

void setup() {
  pinMode(7, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(A8, OUTPUT);
  Serial.begin(9600);

  TCCR4B = (TCCR4B & 0xF8) | 4;
}

void loop() {
  digitalWrite(A7, HIGH);
  digitalWrite(A8, LOW);
//  for(float i = 0; i < 1; i = i + 0.1){
//    analogWrite(7, int(val*i));
//    Serial.println(int(val*i));
//    delay(1000);
//  }
//
//  digitalWrite(A7, LOW);
//  digitalWrite(A8, HIGH);
//  for(float i = 0; i < 1; i = i + 0.1){
//    analogWrite(7, int(val*i));
//    Serial.println(int(val*i));
//    delay(1000);
//  }
  analogWrite(7, i++);
  Serial.println(i);
  delay(1000);
}
