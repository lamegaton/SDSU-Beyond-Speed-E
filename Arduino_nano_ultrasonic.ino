/*
 * created by Rui Santos, https://randomnerdtutorials.com
 * 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC (RED)
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND (GREEN)
 */
#include "pitches.h"

int trigPin = 8;    // Trigger
int echoPin = 9;    // Echo
int speaker = 0;
long duration, cm, inches, temp;
//MAX AND MIN VALUE FOR CALIBRATION
long sensorMin = 10000;
long sensorMax = 0;
int offset,i,j = 0;

//speaker at pin 8
void playSound(){
  // Play coin sound   
  tone(speaker,NOTE_B5,100);
  delay(100);
  tone(speaker,NOTE_E6,850);
  delay(800);
  noTone(speaker);
}

void readSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  //Serial.print(inches);
  //Serial.print("in, ");
//  Serial.print(cm);
//  Serial.print("cm");
//  Serial.println();
}

void calibration(){
  pinMode(13, OUTPUT);
  
  while(millis() < 6000){
    digitalWrite(13, HIGH);
    readSensor();
    //find max
    if (cm > sensorMax) {
      sensorMax = cm;
    }
    //find min
    if (cm < sensorMin) {
      sensorMin = cm;
    }
    delay(150);
    digitalWrite(13, LOW);
    offset = sensorMax - sensorMin;
  }
  if (sensorMin == 0){sensorMin = sensorMax - 100;}
  Serial.print("Max distance: ");Serial.println(sensorMax);
  Serial.print("Min distance: ");Serial.println(sensorMin);
}

void setup() {
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  calibration();
}
 
void loop() {
  readSensor();
  if(cm < sensorMin){
    temp = i;
    i+=1;
    Serial.print(i);
    Serial.println(" --- CAR DETECTED!");
  }
  else{
    i=0;
    if (temp > i){
    j+=1;
    //playSound();
    Serial.print("CAR #");
    Serial.println(j);
    temp = i;
    }
  }
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  delay(50);
}
