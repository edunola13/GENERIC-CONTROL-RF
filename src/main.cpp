//0= Una unica placa, 1= Placas interconectadas
#define MODE_CORE 0

#include "Arduino.h"
// Uncomment to enable printing out nice debug messages.
#define DOMO_DEBUG
#define DOMO_SPEED 9600
//#define USE_WDT
//#define WDT_TIME WDTO_8S

#include <common_initial.h>
#include "messages.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define Joystick Connections
#define JoyStick_X_PIN A1
#define JoyStick_Y_PIN A0
#define JoyStick_B_PIN 3

// Declare unsigned 8-bit joystick array
int joystick[3];

RF24 radio(9, 10);
struct MESSAGE {
  char action;
  int data[4];
};
MESSAGE data = {};
byte slaceAdd[5] = {'R', 'x', 'A', 'A', 'A'};

void setup() {
  initialGeneric();
  DEB_DO_PRINTLN(MSG_START);

  pinMode(JoyStick_B_PIN, INPUT_PULLUP);

  // Interface RF24
  DEB_DO_PRINTLN("Radio Begin");
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.setRetries(3, 5);
  radio.openWritingPipe(slaceAdd);
  DEB_DO_PRINTLN("Radio Conf");
}

void loop() {
  // Read Joystick values and map to values of 0 - 255
  joystick[0] = map(analogRead(JoyStick_X_PIN), 0, 1023, 0, 255);
  joystick[1] = map(analogRead(JoyStick_Y_PIN), 0, 1023, 0, 255);
  joystick[2] = digitalRead(JoyStick_B_PIN) == LOW ? 1 : 0; // LOW = Se apreto

  data.action = 'a';
  data.data[0] = joystick[0];
  data.data[1] = joystick[1];
  data.data[2] = joystick[2];
  DEB_DO_PRINTLN(data.data[0]);
  DEB_DO_PRINTLN(data.data[1]);
  DEB_DO_PRINTLN(data.data[2]);

  bool rta = radio.write(&data, sizeof(data));
  if (rta) {
    if (radio.isAckPayloadAvailable()) {
      int ack;
      radio.read(&ack, sizeof(ack));
    } else {
      DEB_DO_PRINTLN("Fail Ack");
    }
  } else {
    DEB_DO_PRINTLN("Tx Failed");
  }

  delay(100);
}
