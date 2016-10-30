#include <Bridge.h>
#include <HttpClient.h>

int state = 0; //open

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  Serial.begin(9600);
  //while(!Serial);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (analogRead(A0) > 300) {
    // open - no magnet
    if (state == 1) {
      HttpClient client;
      client.get("http://www.arduino.cc/asciilogo.txt");

      while (client.available()) {
        //Serial.println("state changed from closed to open");
        char c = client.read();
        Serial.print(c);
      }
      Serial.flush();
    }
    state = 0;
    //Serial.println(".");
    analogWrite(3, 0);
    analogWrite(5, 100);
  } else {
    // closed - magnet
    state = 1;
    //Serial.println("=");
    analogWrite(3, 100);
    analogWrite(5, 0);
  }
  delay(10);
}
