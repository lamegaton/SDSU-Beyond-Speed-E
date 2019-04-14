//PIN FOR SENSOR
#define TRIGGER 8 //orange 
#define ECHO 9 //yellow

long duration;
long distance;
long distanceThreshold = 0; //10 feet range threshold
int vehicleCount = 0;
int Vehicle_Detected_Confidence_Level = 0;
//MAX AND MIN VALUE FOR CALIBRATION
long sensorMin = 10000;
long sensorMax = 0;

void calibration(){
  pinMode(13, OUTPUT);
  while(millis() < 10000){
    digitalWrite(13, HIGH);
    readSensor();
    //find max
    if (distance > sensorMax) {
      sensorMax = distance;
    }
    //find min
    if (distance < sensorMin && distance != 0) {
      sensorMin = distance;
    }
  }
  digitalWrite(13, LOW);
  Serial.print("Max distance: ");Serial.println(sensorMax);
  Serial.print("Min distance: ");Serial.println(sensorMin);
  distanceThreshold = sensorMin - 2;
}

void readSensor(){
  digitalWrite(TRIGGER, LOW); // Setting Trigger Pin Low for 2 microseconds
  delayMicroseconds(2);

  digitalWrite(TRIGGER, HIGH);  // Setting Trigger Pin High for 10 microseconds to trigger ranging
  delayMicroseconds(10);

  digitalWrite(TRIGGER, LOW); // Setting Trigger Pin Low
  
  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.00676;
}

void countCar(){
  if (distance <= distanceThreshold)
  {
    Serial.print("Vehicle Detected ");
    Serial.print(distance);
    Serial.println(" inches away");
    Vehicle_Detected_Confidence_Level ++;
  }
  else 
  {
    if (Vehicle_Detected_Confidence_Level > 3) {
      vehicleCount ++;
      Serial.print("Vehicle passed. Vehicle Count: ");
      Serial.println(vehicleCount);
    } 
    Vehicle_Detected_Confidence_Level = 0;
  }  
}

void setup(){
  Serial.begin(9600); //Baudrate
  pinMode(TRIGGER, OUTPUT); // Assigning Trigger Pin as Output
  pinMode(ECHO, INPUT); // Assigning Echo Pin as Input
  calibration();
}

void loop()
{
  readSensor();
  countCar();
}
