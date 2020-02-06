#include "HX711.h"
#include <DallasTemperature.h>
#include <OneWire.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>
#include <string.h>
#include <stdio.h>

#define STRLEN 128

//Load sensor
#define LOAD_DOUT 6
#define LOAD_CLK 5
HX711 scale;
float calibration_factor = -7050;

//Temp sensor
#define ONE_WIRE_BUS 2
DeviceAddress thermoAddr = { 0x28, 0xA6, 0x44, 0x94, 0x97, 0x0E, 0x03, 0x85 };
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//Tranciever
const uint16_t RADIO_CE = 7;
const uint16_t RADIO_CS = 8;
const byte address[6] = "00001";
RF24 radio(RADIO_CE, RADIO_CS);

//Define functions
float getTemp();

void setup() {
  
  //Set up functions
  Serial.begin(9600);

  sensors.begin();
  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  
  double temp = 0.0;
  
  //get temperature
  temp = getTemp();
  if (temp == -127.00) {  }
  else {
    
//    //send to key-fob
//    radio.write(&send, sizeof(send));
//    delay(1000);

    Serial.print(temp);
  }
  Serial.print("\n\r");
  delay(500);
}

float getTemp() {

  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}
