#include <Bridge.h>
#include <HttpClient.h>
#include "config.h"

#define STATE_OPEN 0
#define STATE_CLOSED 1

int state = STATE_OPEN;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  Serial.begin(9600);
}

void changeLED(int red, int green, int blue) {
  analogWrite(3, red);
  analogWrite(5, green);
  analogWrite(6, blue);
}
void loop() {
  if (analogRead(A0) > 300) {
    // open - no magnet
    changeLED(0, 100, 0);
    
    // send notification if previous state was closed
    if (state == STATE_CLOSED) {
      HttpClient client;
      client.get(URL);
      while (client.available()) {
        //Serial.print("client available");
        char c = client.read();
        Serial.print(c);
      }
      state = STATE_OPEN;
      Serial.flush();
    }
    
  } else {
    // closed - magnet
    changeLED(100, 0, 0);
    state = STATE_CLOSED;
  }
  
  delay(10);
}
