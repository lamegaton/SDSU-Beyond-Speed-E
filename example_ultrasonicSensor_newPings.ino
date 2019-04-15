/* JSN-SR04T Ultrasonic Distance Sensor with NewPing library Example Code, More info: https://www.makerguides.com */
#include <NewPing.h>
//Define Trig and Echo pin
#define trigPin 8
#define echoPin 9
// Define maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define MAX_DISTANCE 12*14
//NewPing setup of pins and maximum distance.
NewPing sonar(trigPin, echoPin, MAX_DISTANCE); 
int distance = 0;
void setup()
{
Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.
}
void loop()
{
    delay(50); //Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    distance = sonar.ping_in();
  if (distance > 0){
    Serial.print("Distance = ");
    Serial.print(distance); // Send ping, get distance in cm and print result (0 = outside set distance range)
    Serial.println(" in");
  }
}
