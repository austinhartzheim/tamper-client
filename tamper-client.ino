#include <Bridge.h>
#include <HttpClient.h>
#include <stdio.h>
#include "config.h"

#define STATE_OPEN 0
#define STATE_CLOSED 1

int state = STATE_OPEN;
int sessionID;
char urlBuffer[URL_BUFFER_SIZE];
unsigned long lastPing = millis(); 
HttpClient client;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  Serial.begin(9600);
  randomSeed(analogRead(1));
  sessionID = random(500);
  ping();
}

void ping() {
  snprintf(urlBuffer, 300, PING_URL, sessionID);
  Serial.println(urlBuffer);
  client.get(urlBuffer);
  sessionID++;
}

void changeLED(int red, int green, int blue) {
  analogWrite(3, red);
  analogWrite(5, green);
  analogWrite(6, blue);
}

void loop() {

  unsigned long currentTime = millis();
  
  if (analogRead(A0) > 300) {
    // open - no magnet
    changeLED(0, 100, 0);
    
    // send alert notification if box is opened
    if (state == STATE_CLOSED) {
      client.get(ALERT_URL);
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
      }

      // updating at the end to detect state transition
      state = STATE_OPEN;
      Serial.flush();
    }
    
  } else {
    // closed - magnet
    changeLED(100, 0, 0);

    // begin ping notifications when box is locked
    if ( state == STATE_OPEN || (currentTime-lastPing)>PING_INTERVAL ) {
      ping();
      lastPing = currentTime;
    }
    
    // updating at the end to detect state transition
    state = STATE_CLOSED;
  }
  
  delay(10);
}
