// Adafruit IO Subscription Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
#include "config.h"

#define redPin  16
#define bluePin 15

#define startHour 8 // when should the light turn on

/************************ Example Starts Here *******************************/

// set up the feeds
AdafruitIO_Feed *blue   = io.feed("Blue");
AdafruitIO_Feed *red    = io.feed("Red");
AdafruitIO_Feed *ontime = io.feed("On Time");
AdafruitIO_Time *iso    = io.time(AIO_TIME_ISO);

// variables
int redIntensity, blueIntensity, onTimeValue;

void setup() {
  // initialise LEDs pins as outputs
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  analogWrite(redPin, 100);
  analogWrite(bluePin, 100);

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);

  Serial.print("Connecting to Adafruit IO");

  // start MQTT connection to io.adafruit.com
  io.connect();

  // set up message handlers for the feeds.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  blue->onMessage(handleBlue);
  red->onMessage(handleRed);
  iso->onMessage(handleISO);
  ontime->onMessage(handleOnTime);

  // wait for an MQTT connection
  // NOTE: when blending the HTTP and MQTT API, always use the mqttStatus
  // method to check on MQTT connection status specifically
  while (io.mqttStatus() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Because Adafruit IO doesn't support the MQTT retain flag, we can use the
  // get() function to ask IO to resend the last value for this feed to just
  // this MQTT client after the io client is connected.
  blue->get();
  red->get();
  ontime->get();

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // Because this sketch isn't publishing, we don't need
  // a delay() in the main program loop.
}

void handleRed(AdafruitIO_Data *data) {

  // print out the received light value
  Serial.print("received <- red ");
  String received = data->value();
  int redValue = received.toInt();
  Serial.print(redValue);
  redIntensity = map(redValue, 0, 100, 0, 700);
  Serial.print(" mapped: ");
  Serial.println(redIntensity);

}

void handleBlue(AdafruitIO_Data *data) {

  // print out the received light value
  Serial.print("received <- blue ");
  String received = data->value();
  int blueValue = received.toInt();
  Serial.print(blueValue);
  blueIntensity = map(blueValue, 0, 100, 0, 700);
  Serial.print(" mapped: ");
  Serial.println(blueIntensity);

}

void handleOnTime(AdafruitIO_Data *data) {

  // print out the received light value
  Serial.print("received <- ontime ");
  String received = data->value();
  onTimeValue = received.toInt();
  Serial.print(onTimeValue);

}

void handleISO(char *data, uint16_t len) {

  Serial.print("ISO Feed: ");
  String strdata = data;
  String h = strdata.substring(11, 13);
  int hour = h.toInt() + 2;
  Serial.println(hour);

  if ((hour >= startHour) && hour <= (startHour + onTimeValue)) {
    analogWrite(bluePin, blueIntensity);
    analogWrite(redPin, redIntensity);
  } else {
    analogWrite(bluePin, 0);
    analogWrite(redPin, 0);
  }

}
