/*
 * PIR sensor tester
 */

#include <pitches.h>

byte printState = 0; //0- print output change from LOW to HIGH or viceversa ; 1 - print state
byte ledPin = 13;                // choose the pin for the LED
byte inputPin = 2;               // choose the input pin (for PIR sensor)
byte pirState = LOW;             // we start, assuming no motion detected
byte val = 0;                    // variable for reading the pin status
byte speaker = 8;
 
void playSound(){
  // Play coin sound   
  tone(speaker,NOTE_B5,100);
  delay(100);
  tone(speaker,NOTE_E6,850);
  delay(800);
  noTone(speaker);
}


void setup() {
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
 
  Serial.begin(115200);
}
 
void loop(){
  val = digitalRead(inputPin);  // read input value
  switch (printState){
    case 0:
    if (val == HIGH) {            // check if the input is HIGH
      digitalWrite(ledPin, HIGH);  // turn LED ON
      if (pirState == LOW) {// that means it switch from LOW to HIGH to avoid printing extra print
        // we have just turned on
        Serial.println("Motion detected!");
        // We only want to print on the output change, not state
        pirState = HIGH;
        //playSound();
        
      }
    } else {
      digitalWrite(ledPin, LOW); // turn LED OFF
      if (pirState == HIGH){// read LOW
        // we have just turned of
        Serial.println("Motion ended!");
        // We only want to print on the output change, not state
        pirState = LOW;
      }
      break;
    case 1:
      Serial.println(val);
      break;
    }
  }
}
