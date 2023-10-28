#include <Arduino.h>
#define RELAY_PIN 6
#define AC_mon 7
void allow_access(){
  int comp = 0;
  boolean start = false;
  while (comp!=3){
    int ZC_value = analogRead(AC_mon);

    if(ZC_value < 500){start = true;}
    if (ZC_value == 5000){
      if(start == true){
        delay(22);
        digitalWrite(RELAY_PIN, HIGH);
        comp = 3;
      }
      
    }
    Serial.println(ZC_value);
  }
  
}
void setup() {
  Serial.println("Setting up");
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT); 
  delay(4000);
  Serial.println("Setup Complete");
  allow_access();
}

void loop() {
  // put your main code here, to run repeatedly:
}