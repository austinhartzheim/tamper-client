#include <Bridge.h>
#include <HttpClient.h>

#define STATE_OPEN 0
#define STATE_CLOSED 1
#define URL "https://<TODO>.amazonaws.com/prod/event/box_opened"

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

    // send notification if previous state was closed
    if (state == STATE_CLOSED) {
      HttpClient client;
      client.get("http://www.arduino.cc/asciilogo.txt");
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
      Serial.flush();
    }
    
    state = STATE_OPEN;
    changeLED(0, 100, 0);
    
  } else {
    // closed - magnet
    state = STATE_CLOSED;
    changeLED(100, 0, 0);
  }
  
  delay(10);
}
