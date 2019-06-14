/*
  Credit to by David A. Mellis  <dam@mellis.org>
  modified 9 Apr 2012 by Tom Igoe
  modified by Son 2019
  This example code is in the public domain.
  http://www.arduino.cc/en/Tutorial/Smoothing
*/
const int numReadings = 100;

float readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average
float cal = 0;
int inputPin = A3;
long result;
float offset0 = 3.5;//3
float offset1 = 0;//0.0005

long readVcc() {
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}
void setup() {
  // initialize serial communication with computer:
  Serial.begin(115200);
  analogReference(INTERNAL);
  delay(100);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
  analogReference(INTERNAL);
  delay(50);
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin) + offset0;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = (total / numReadings);
  Serial.println(average);
  // send it to the computer as ASCII digits
  average = average * 0.001075 + offset1;
  Serial.println(average,4);
  // delay in between reads for stability
}
