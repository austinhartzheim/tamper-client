#include <Bridge.h>
#include <HttpClient.h>
#include <stdio.h>
#include "config.h"

#define TRANSITION_TIMEOUT 1500

enum State {OPEN, TRANSITION1, TRANSITION2, TRANSITION3, CLOSED, WAIT_UNTIL_OPEN};

State state = OPEN;
State lastState = OPEN;

int sessionID;
char urlBuffer[URL_BUFFER_SIZE];
unsigned long lastPing = millis(); 
unsigned long lastStateChange = millis(); 
HttpClient client;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  Serial.begin(9600);
  randomSeed(analogRead(1));
  sessionID = random(500);
  ping(true);
}

void ping(bool forcePing) {
  if (forcePing || (millis()-lastPing)>PING_INTERVAL) {
    snprintf(urlBuffer, 300, PING_URL, sessionID);
    Serial.println(urlBuffer);
    client.get(urlBuffer);
    sessionID++;
    lastPing = millis();    
  } 
}

void changeLED(int red, int green, int blue) {
  analogWrite(3, red);
  analogWrite(5, green);
  analogWrite(6, blue);
}

void changeState(State newState) {
  lastState = state;
  state = newState;
  lastStateChange = millis();
}

// returns true if magnet on lid is detected
bool isMagnetClosed() {
  if (analogRead(A0) > 300) {
    return false;
  }
  return true;
}

void alarm() {
  client.get(ALERT_URL);
    while (client.available()) {
      client.read();
    }
}

void boxLocked() {
  client.get(LOCKED_URL);
  while (client.available()) {
    client.read();
  }
}

void loop() {
  switch (state) {
      case OPEN:
        changeLED(0, 100, 0);
        if (isMagnetClosed()) {
          changeState(TRANSITION1);
        }
        break;
      case TRANSITION1:
        changeLED(0, 0, 25);
        if (millis() - lastStateChange > TRANSITION_TIMEOUT) {
          changeState(WAIT_UNTIL_OPEN);
        }
        if (!isMagnetClosed()) {
          changeState(TRANSITION2);
        }
        break;
      case TRANSITION2:
        changeLED(0, 0, 100);
        if (millis() - lastStateChange > TRANSITION_TIMEOUT) {
          changeState(WAIT_UNTIL_OPEN);
        }
        if (isMagnetClosed()) {
          changeState(TRANSITION3);
        }
        break;
      case TRANSITION3:
        changeLED(100, 0, 0);
        ping(true);
        boxLocked();
        changeState(CLOSED);      
        break;
      case CLOSED:
        changeLED(100, 0, 0);
        if (!isMagnetClosed()) {
          alarm();
          changeState(OPEN);
        }
        ping(false);
        break;
      case WAIT_UNTIL_OPEN:
        changeLED(100,100,100);
        if (!isMagnetClosed()) {
          changeState(OPEN);
        }
        break;
      default:
        break;
    }
}
