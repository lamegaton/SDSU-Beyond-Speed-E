/********************************
 * ENABLE CODE
 ********************************/
//PIN FOR SENSOR
#define TRIGGER 8 //orange 
#define ECHO 9 //yellow

//ENABLE FUNCTIONS
#define ABP = 1
#define OTAA = 0
#define SENSOR_ONLY 1

#if SENSOR_ONLY == 0
//Cayenne
#include <CayenneLPP.h>
//Lib for lora
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
//#include <node_config.h>
#endif

/********************************
 * DEFINE VARIABLES FOR SENSOR
 * ******************************/
byte offset = 2;
long duration;
long distance;
long distanceThreshold = 12*14; //14 feets
int vehicleCount = 0;
int Vehicle_Detected_Confidence_Level = 0;
//MAX AND MIN VALUE FOR CALIBRATION
long sensorMin = 10000;
long sensorMax = 0;

/********************************
 * FOR NUMBER #2 COMM-SETTING
 * *****************************/
#if SENSOR_ONLY == 0
  
/********************************
 * Setup for Cayenne
 * Read more: https://www.thethingsnetwork.org/docs/devices/arduino/api/cayennelpp.html
 * 
 ********************************/
  CayenneLPP lpp(10);//51 bytes maximum payload
  static const PROGMEM u1_t NWKSKEY[16] ={ 0x37, 0x23, 0x94, 0xFF, 0xE8, 0x21, 0x5A, 0x15, 0xB2, 0x10, 0xC4, 0x65, 0xF8, 0x1A, 0x40, 0x6F };
  static const u1_t PROGMEM APPSKEY[16] ={ 0x9A, 0xD4, 0x8C, 0x75, 0xEF, 0x13, 0x9B, 0x2B, 0x3D, 0x41, 0x0B, 0x37, 0xE1, 0x8D, 0x47, 0xBB };
  static const u4_t DEVADDR = 0x260217D3 ; 
  
  // These callbacks are only used in over-the-air activation
  void os_getArtEui (u1_t* buf) { }
  void os_getDevEui (u1_t* buf) { }
  void os_getDevKey (u1_t* buf) { }
  
  //static uint8_t mydata[] = "from SDSU";
  static uint8_t mydata[2];
  
  static osjob_t sendjob;
  
  // Schedule TX every this many seconds
  const unsigned TX_INTERVAL = 10;
  
  // Pin mapping for arduino nano
  const lmic_pinmap lmic_pins = {
      .nss = 10,//cs
      .rxtx = LMIC_UNUSED_PIN,
      .rst = 2,
      .dio = {6,7,LMIC_UNUSED_PIN},
  };

//NUMBER 2 COMM FUNCTION
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
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE://processing downlink
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
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
            Serial.println((unsigned) ev);
            break;
    }
}
/********************************
 * LOOP FOR LORA / TX
 * *****************************/
 
void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        /*
        //use without cayenne
        mydata[0]= vehicleCount;
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        */
        LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), 0);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void iniLora(){
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are be provided.
    #ifdef PROGMEM
    // On AVR, these values are stored in flash and only copied to RAM
    // once. Copy them to a temporary buffer here, LMIC_setSession will
    // copy them into a buffer of its own again.
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x13, DEVADDR, nwkskey, appskey);//can be 0x1
    #else
    // If not running an AVR with PROGMEM, just use the arrays directly
    LMIC_setSession (0x13, DEVADDR, NWKSKEY, APPSKEY);
    #endif

    #if defined(CFG_us915)
    // First disable all sub-bands
    for (int b = 0; b < 8; ++b) {
      LMIC_disableSubBand(b);
    }
    // Then enable the channel(s) you want to use
    LMIC_enableChannel(8); // 903.9 MHz
    //LMIC_selectSubBand(1);
    #endif
    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // TTN uses SF9 for its RX2 window.
    LMIC.dn2Dr = DR_SF9;

    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7,14);

    // Start job
    do_send(&sendjob);
  }
#endif
/********************************
 * CALIBRATION
 ********************************/
/*
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
  Serial.print("Max: ");Serial.println(sensorMax);
  Serial.print("Min: ");Serial.println(sensorMin);
  distanceThreshold = sensorMin - 2;
}
*/
void readSensor(){
  digitalWrite(TRIGGER, LOW); // Setting Trigger Pin Low for 2 microseconds
  delayMicroseconds(5);

  digitalWrite(TRIGGER, HIGH);  // Setting Trigger Pin High for 10 microseconds to trigger ranging
  delayMicroseconds(20);

  digitalWrite(TRIGGER, LOW); // Setting Trigger Pin Low
  
  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.00676;
  //Serial.println(distance);
}

/********************************
 * COUNT VEHICLE FUNCTION
 ********************************/
void countCar(){
  if (distance < distanceThreshold)
  {
    Vehicle_Detected_Confidence_Level ++;
  }
  else 
  {
    //Serial.println('0');
    if (Vehicle_Detected_Confidence_Level >= 5) {
      vehicleCount ++;
      //Serial.print("Vehicle #: ");
      Serial.print("Vehicle Count ");
      Serial.print(vehicleCount);    
      Serial.print(" Confidence level: "); 
      Serial.println(Vehicle_Detected_Confidence_Level);
    } 
    Vehicle_Detected_Confidence_Level = 0;
  }
  #if SENSOR_ONLY == 0
  lpp.reset();
  lpp.addPresence(1,vehicleCount);  
  #endif
}

void setup() {
    pinMode(TRIGGER, OUTPUT);
    pinMode(ECHO, INPUT);
    Serial.begin(115200);
    //calibration();
    Serial.println(F("Starting"));
    #if SENSOR_ONLY == 0
    iniLora();
    #endif
    
}

void loop() {
    readSensor();
    countCar();
    #if SENSOR_ONLY == 0
    os_runloop_once();
    #endif
}
