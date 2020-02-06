#include <HX711.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>
#include <string.h>
#include <stdio.h>

//Load sensor
#define LOAD_DOUT 6
#define LOAD_CLK 5
HX711 scale;

void setup() {

  Serial.begin(9600);
  scale.begin(LOAD_DOUT, LOAD_CLK);
  scale.set_scale();
  scale.tare();
}

void loop() {

  float current = scale.get_units(10);
  float factor = (current/0.180);

  Serial.println(factor);
}
