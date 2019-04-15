/* JSN-SR04T Ultrasonic Distance Sensor with Arduino Example Code. More info: https://www.makerguides.com */
//Define Trig and Echo pin
#define trigPin 8
#define echoPin 9
//Define variables
long duration;
long distance;
void setup()
{
//Define inputs and outputs
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
//Begin Serial communication
Serial.begin(9600); // Starts the serial communication at a baudrate of 9600
}
void loop()
{
//Clear the trigPin by setting it LOW
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
//Trigger the sensor by setting the trigPin high for 10 microseconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(20);
digitalWrite(trigPin, LOW);
//Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds.
duration = pulseIn(echoPin, HIGH);
// Calculate the distance
distance= (duration*0.034/2)/2.54;
//Print the distance on the Serial Monitor (Ctrl+Shift+M)
Serial.print("Distance = ");
Serial.print(distance);
Serial.println(" in");
delay(100);
}
