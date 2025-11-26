#include <assert.h>

#include "Magnet.h"

const int RedLED = 22;
const int BlueLED = 23;
const int GreenLED = 6;
Magnet myMagnet(0, 0, RedLED, BlueLED, GreenLED);

void setup() {
  Serial.begin(9600);
  Serial.println(myMagnet.get_row() == 0);
  Serial.println(myMagnet.get_column() == 0);
  Serial.println(myMagnet.get_VoutPin() == GreenLED);
  Serial.println(myMagnet.get_upPin() == RedLED);
  Serial.println(myMagnet.get_downPin() == BlueLED);
  Serial.println(myMagnet.is_active() == false);
  Serial.println(myMagnet.get_voltage() == 0);
  Serial.println(myMagnet.get_intensity() == 0);
}

void loop() {
  for(float i = 0; i < 2*PI; i = i + (PI/8)){
    Serial.println("SENT VOLTAGE: " + (String)sin(i));
    myMagnet.set_voltage(sin(i));
    Serial.println("GOTTEN VOLTAGE: " + (String)myMagnet.get_voltage());
    Serial.println("GOTTEN INTENSITY: " + (String)myMagnet.get_intensity());
    delay(500);
  }
  for(float i = 0; i < 2*PI; i = i + PI/8){
    Serial.println("SENT INTENSITY: " + (String)(100*sin(i)));
    myMagnet.set_intensity(100*sin(i));
    Serial.println("GOTTEN VOLTAGE: " + (String)myMagnet.get_voltage());
    Serial.println("GOTTEN INTENSITY: " + (String)myMagnet.get_intensity());
    delay(500);
  }
}
