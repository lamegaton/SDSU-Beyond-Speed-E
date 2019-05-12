/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <LoraMessage.h>

#include "LowPower.h"
//#include <util/atomic.h> //Only needed for full version to update time

volatile boolean powerdown=false;

unsigned int Vbat;
unsigned int Vsensor;

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = { };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = {  };
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = {  };
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}

static osjob_t sendjob;

#define TX_INTERVAL 10           //seconds of sleep between sends, originally 3600
#define SLEEPCYLCES TX_INTERVAL/8  // calculate the number of sleepcycles (2s/8s) needed for the given TX_INTERVAL

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 6,
  .dio = {2, 5, LMIC_UNUSED_PIN},
};


void onEvent (ev_t ev) {
    Serial.print((unsigned long)(os_getTime()/ OSTICKS_PER_SEC));
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
            /*
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              Serial.print("netid: ");
              Serial.println(netid, DEC);
              Serial.print("devaddr: ");
              Serial.println(devaddr, HEX);
              Serial.print("artKey: ");
              for (int i=0; i<sizeof(artKey); ++i) {
                Serial.print(artKey[i], HEX);
              }
              Serial.println("");
              Serial.print("nwkKey: ");
              for (int i=0; i<sizeof(nwkKey); ++i) {
                Serial.print(nwkKey[i], HEX);
              }
              Serial.println("");
            }
            */
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
            // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;

    case EV_TXCOMPLETE:
      
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));

        if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));

        Serial.println(F("sent"));

      //send/receive cycle completed
      powerdown=true;
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
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
        //case EV_TXSTART:
         //   Serial.println(F("EV_TXSTART"));
          //  break;
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}

void do_send(osjob_t* j) {
  
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {

    //Prepare data
    LoraMessage message;

    message.addUint16(Vsensor);
    //message.addUint8("ID:0A,S:200,B:4.20,4:42PM");
    //static uint8_t mydata[] = "ID:0A,S:200,B:4.20,4:42PM";
    // Prepare upstream data transmission at the next possible time.
  
    Serial.print((unsigned long)(os_getTime()/ OSTICKS_PER_SEC));
    Serial.print(": ");
    Serial.println(F("Queue packet"));
    
    LMIC_setTxData2(1, message.getBytes(), message.getLength(), 0);    

  }
  // Next TX is scheduled after TX_COMPLETE event.
}



void setup() {

  Serial.begin(115200);
  Serial.println(F("Starting"));

  // LMIC init
  os_init();

  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);  //Relax RX timing window

  // Start job (sending automatically starts OTAA too)
  measure();  //Do a first measurement at startup
  powerdown=false;
  os_setTimedCallback(&sendjob, os_getTime() + ms2osticks(10), do_send);
}

void loop() {

  extern volatile unsigned long timer0_overflow_count;
  os_runloop_once();  //check send status

  if (powerdown) {

    Serial.println(F("go to sleep ... "));
    Serial.flush();
    
    for (int i=0; i<SLEEPCYLCES; i++) {
      // Enter power down state for 8 s with ADC and BOD module disabled 
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      //LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
            
      //Give the AVR back the slept time back (simple version)
      cli();
      timer0_overflow_count += 8 * 64 * clockCyclesPerMicrosecond(); //give back 60 seconds of sleep
      sei();

      /*      
      //Give the AVR back the sleep time; full version with millis() update
      //Needs '#include <util/atomic.h>'
      unsigned long slept=8*1000;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        extern volatile unsigned long timer0_millis;
        extern volatile unsigned long timer0_overflow_count;
        timer0_millis += slept;
        // timer0 uses a /64 prescaler and overflows every 256 timer ticks
        timer0_overflow_count += microsecondsToClockCycles((uint32_t)slept * 1000) / (64 * 256);
      }
      */     
      
      os_getTime();   //VERY IMPORTANT after sleep to update os_time and not cause txbeg and os_time out of sync which causes send delays with the RFM95 on and eating power

      //Do here whatever needs to be done after each of the sleepcycle (e.g. check for a condition to break for send or take measurements for mean values etc.)
    }

    //Instead of the for-loop, a SLEEP_FOREVER can be used with waking by interrupt (if defined)
    //LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); //Also only add e.g. 1 minute after wake to time0_overflow_count if this is used
    //os_getTime();   //VERY IMPORTANT after sleep to update os_time and not cause txbeg and os_time out of sync which causes send delays with the RFM95 on and eating power 

    Serial.begin(115200);
    Serial.println(F("... done sleeping")); 

    powerdown=false;
    measure();  //get some data
    os_setTimedCallback(&sendjob, os_getTime() + ms2osticks(10), do_send);  //do a send
  }

} //loop


void measure() {
  //Assumes you have a voltage divider at A0. Preferrably one that does not drain battery, like this one:
  //https://jeelabs.org/2013/05/16/measuring-the-battery-without-draining-it/
   Vsensor = analogRead(A0);
   Serial.println(Vsensor);

  //Vbat = analogRead(A0) / 1024.0 * 3300 * 2;  //Assuming you run on 3.3V
}
