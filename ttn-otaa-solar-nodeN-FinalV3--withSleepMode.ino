#define debug 0 //enable serial.print functions
#define dummy 0 //enable dummy variables 
#define solarActivate 1
#define clockActivate 0

/*** SENSOR ***/
// CLOCK
#include <Wire.h>
#include <RTClibExtended.h> // for clock
//#include "RTClib.h"

/*** SLEEP MODE ***/
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// INA219
#include <Adafruit_INA219.h>

/*** COMMUNICATION ***/
#include <lmic.h> 
#include <hal/hal.h> 
#include <SPI.h>

// LORAMESSAGE
#include <LoraMessage.h>
#define NODE 2// 2 for testing
#include "C:\Users\mapnh\Documents\Arduino\solarNodes_lib\pin_mapping.h" 

LoraMessage message;
RTC_DS3231 rtc; // declare clock object
Adafruit_INA219 ina219;

/****************************************
 * DEFINE VARIABLES FOR SENSOR AND LORA *
 * **************************************/
byte i=0;
byte t=0;
byte sec = 20;//60
byte mins = 1;
//**** LoraMessage package ****//
unsigned int Vbaterry = 0;
unsigned int Vpanel = 0;
unsigned long startTimer = 0;
unsigned long currentTimer = 0;
unsigned long milisecs = 20000;

/** Float variable for LoraMessage **/
float Vpyranometer_float =  0;
float Ipanel_float = 0;
float Radiation = 0;
volatile byte f_wdt = 1;
volatile byte sleepCounter = 0;

static osjob_t sendjob;
/*********************************
 * FUNCTIONS FOR SENSOR AND LORA *
 * *******************************/

/*** ISR(WDT_vect) watchdog interrupt service ***/
ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    Serial.println("WDT Overrun!!!");
  }
}

void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  /* Now enter sleep mode. */
  sleep_mode();
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  /* Re-enable the peripherals. */
  power_all_enable();
}

void init_sleepMode(){
  /*** Setup the WDT ***/
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
}

/* PYRANOMETER */
// 1000W / 5 = 200mV so just want to make sure it's accurate in the range 0 - 200mV
float readPyranometer_float(){
  const int numReadings = 200;// if this is set to 300 it will be overflow
  int readings[numReadings];      // the readings from the analog input
  int readIndex = 0;              // the index of the current reading
  float total = 0;                  // the running total
  float average = 0;                // the average
  byte offset0 = 0;//300/10, 200/9, 13 when run with other sensor only,
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {readings[thisReading] = 0;}
  for(int z = 0; z < 10; z++){analogReference(INTERNAL);} // change ref voltage to 1.1 V, or DEFAULT
  delay(10);
  
  while(readIndex < numReadings){
    total = total - readings[readIndex];
    readings[readIndex] = analogRead(pyrano) + 5;
//    Serial.println("step: ");
//    Serial.println(readings[readIndex]);delay(50);
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    average = (total / numReadings)* (1073/1023.00) ; //14  11.5
    //average = (total / numReadings)* (5000.00/1023.00) + 0; 
    delay(1);
  }
//  Serial.print("Average without offset   ");
//  Serial.println(average);
  if(average > 200){average = average * 1.017;}
  else if(average < 5.25){average = 1;}
  else{
  average = average * 1.02463;
  readIndex = 0;
  }
  Radiation = average * 50;
  return average;
  // we have to take average value and minus 10 when we test it with usb cable
}
/* INA219 FUNCTION */
void ina219_init(void) {
  uint32_t currentFrequency;  
  ina219.begin();
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
}
//check 06/24/19
void readBattery(){
  analogReference(DEFAULT);
  int sensorValue = analogRead(battery);
  float voltage = sensorValue * (5.0 / 1023.0);
  Vbaterry = (voltage - 0.20) * 1000;// -0.30 offset
}

void readPanel() {
  int busvoltage_mV = 0;
  float current_mA = 0;


  busvoltage_mV = ina219.getBusVoltage_raw();//we have to calculate in server side by multiply with 0.001 V
  current_mA = ina219.getCurrent_mA();

  Ipanel_float = abs(current_mA);
  Vpanel = busvoltage_mV;

}

/* COMMUNICATION FUNCTIONS */
// Working fine 06/17
void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));

            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
            break;
        case EV_TXCOMPLETE:
              Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));//commented out on june 24 2019
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // Schedule next transmission
            //os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
         default:
            Serial.println(F("Unknown event"));
            break;
    }
}

// Working fine 06/17
void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // prepare packet for LoraMessage
          message
              .addUint16(Vbaterry)
              .addUint16(Vpanel)
              .addHumidity(Ipanel_float)
              .addHumidity(Vpyranometer_float)
	            .addHumidity(Radiation);

        // Prepare upstream data transmission at the next possible time.
        //LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);//default
        LMIC_setTxData2(1, message.getBytes(), message.getLength(), 0);//this is for LoraMessage
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    Serial.println(F("Starting"));
    /* Clock init **/ 
    Wire.begin();
    rtc.begin();
    /* INA219 init */
    ina219_init();
    /* LMIC init */ 
    os_init();
    LMIC_reset();// Reset the MAC state
    LMIC_setAdrMode(false);// Disable adaptive data rate       
    // Send
    os_setTimedCallback(&sendjob, os_getTime() + us2osticks(20), do_send);
    delay(100);
}

void loop() {
  if(f_wdt == 1){
    digitalWrite(ledPin, !digitalRead(ledPin));
    f_wdt = 0;
    if(sleepCounter < 4){
      sleepCounter += 1;
      enterSleep();
    }
  }else{
    /* Do nothings. */  
     sleepCounter = 0;
     #if dummy == 1
      Vpyranometer_float = random(12);
      Vbaterry = random(12);
      Vpanel = random(12);
      Ipanel_float = random(12);
    #else
      os_runloop_once();
      readBattery();
      readPanel();
      Vpyranometer_float = readPyranometer_float();
    #endif
    currentTimer = millis() - startTimer;
    Serial.println(currentTimer);
    #if solarActivate == 1
    if(Vpyranometer_float > 1 && currentTimer > milisecs){
          startTimer = millis();
          os_setTimedCallback(&sendjob, os_getTime() + us2osticks(TX_INTERVAL), do_send);
          message.reset();
          Serial.println(F(" TX_COMPLETE"));
          
    }
    #endif
    #if clockActivate == 1 
    DateTime now = rtc.now();
    t = now.second();
    if(now.hour() >= 23 || now.hour() <= 20){
      if(t % sec == 0 && i == 0){
        os_setTimedCallback(&sendjob, os_getTime() + us2osticks(TX_INTERVAL), do_send);
        message.reset();
        Serial.println(F(" TX_COMPLETE"));
        i = 1;
      }else if(t % sec !=0){
        i = 0;
      }
    }
    #endif
    #if debug == 1
    Serial.print("Pyranometer Value:   ");Serial.println(Vpyranometer_float,2);//Serial.println(" mV");
    Serial.print("Shortwave Radiation: ");Serial.println(Radiation);//Serial.println(" Watt/meter^2");
    Serial.print("Battery Volage (mV): ");Serial.println(Vbaterry);//Serial.println(" mV");
    Serial.print("Solar Panel Voltage: "); Serial.println(Vpanel);//Serial.println(" mV");
    Serial.print("Solar Panel Current: "); Serial.println(Ipanel_float);//Serial.println(" mA");
    Serial.println("");
    #endif
  }
}
