#include "Adafruit_FONA.h"
#include <stdlib.h>
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#include <Wire.h>

// this is a large buffer for replies
char replybuffer[255];
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
// Use this one for FONA 3G
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
uint8_t type;

void setup() {
    attachInterrupt(0, SendMessage, RISING);
    while (! Serial);
    Serial.begin(115200);
    Serial.println(F("Initializing FONA... (May take a few seconds)"));

    fonaSerial->begin(4800);
    if (! fona.begin(*fonaSerial)) {
       Serial.println(F("Couldn't find FONA"));
        while(1);
    }
    Serial.println(F("FONA is OK"));
     // Try to enable GPRS
    Serial.println(F("Enabling GPRS"));  
    fona.enableGPRS(true);
     delay(4000);
 
     Serial.println(F("Enabling GPS..."));
     fona.enableGPS(true);
     delay(4000);

}

void loop() {

     char LAT1[10];//string of lat and long unparsed & overflowing bound
     char LAT[10];
     char LONGI[10];
 
      float latitude, longitude;//variables to hold initial GPS readings
      boolean gps_success = fona.getGPS(&latitude, &longitude);
      if (gps_success) {
          Serial.print("GPS lat:");
          Serial.println(latitude,6);
          Serial.print("GPS long:");
          Serial.println(longitude, 6);
          
      char buffer1[10];
      char buffer2[10];
      String longitu = dtostrf(longitude, 10, 7, buffer1);
      String latitu = dtostrf(latitude, 10, 7, buffer2);
       flushSerial();
      //initialize desired array from unparsed array
      for(int i = 0; i < 9; i++) {
        LAT[i] = LAT1[i];
      }
      LAT[9] = '\0';//truncate array at last desired value
      
      
      Serial.println(LAT);Serial.println(LAT1);Serial.println(longitude);
   

        for (int i = 0; i < 3; i = i + 1)
        {
             if (!fona.enableGPRS(true))
                   Serial.println(F("Failed to turn off"));
             delay(300);
        }
      
        uint16_t statuscode;
        int16_t length;
        char urlString[70];
        String url ="http://www.samthapa.me/test1.php?";
        url += "lat=";
        url +=  latitu;
        url += "&longi=";
        url += longitu;
        url += "&datee=2018-2-22";
        
        url.toCharArray(urlString, url.length());
        flushSerial();
    
        Serial.println(F("****"));
        if (!fona.HTTP_GET_start(urlString, &statuscode, (uint16_t *)&length)) {
              Serial.println("Failed to send to MYSQL database");
 
        }
        while (length > 0) {
          while (fona.available()) {
            char c = fona.read();

            // Serial.write is too slow, we'll write directly to Serial register!
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
            loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
         
            UDR0 = c;
#else
            Serial.write(c);
#endif
            length--;
            if (! length) break;
          }
        }
        Serial.println(F("\n****"));
      }

}
void SendMessage(){
       char message[141];  
       sprintf(message, "  I JUST PUSHED MY PANIC BUTTON! I NEED HELP NOW! http://www.samthapa.me/tracker.php");
       char sendto[13] = "+15628815062";
       fona.sendSMS(sendto, message) ; //sends the message via SMS}   
}     

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

