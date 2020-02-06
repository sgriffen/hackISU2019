#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Time.h>
#include <TimeLib.h>
#include <HX711.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>
#include <string.h>
#include <stdio.h>

#define STRLEN 128

const int RECEIVE = 1000;
const int TRANSMIT = 10000;
const char* ID = "ABCDEFGHI";
char childInCar = '0';

//Load sensor
#define LOAD_DOUT 6
#define LOAD_CLK 5
HX711 scale;
float calibration_factor = -7050;

//Temp sensor
#define ONE_WIRE_BUS A4
DeviceAddress thermoAddr = { 0x28, 0xA6, 0x44, 0x94, 0x97, 0x0E, 0x03, 0x85 };
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensors(&oneWire);

//Tranciever
const uint16_t RADIO_CE = 7;
const uint16_t RADIO_CS = 8;
const byte address[6] = "00001";
long receive_buffer = 0;
long transmit_buffer = 0;
RF24 radio(RADIO_CE, RADIO_CS);

//Ultrasonic sensor
#define SONIC_TRIG A2
#define SONIC_ECHO A3

//Pressure buttons
#define BUTTONS 2

//GPS
#define GPS_RX 5
#define GPS_TX 6
SoftwareSerial gps_serial(GPS_RX, GPS_TX);
TinyGPSPlus gps;
double gps_lat = 41.997745;
double gps_long = -93.632179;

//Define functions
int radio_receive();
void radio_transmit();
float getTemp();
int getSonicDistance();
int getButtons();
void getGPS();
void panic(int flag, float temp);
void dontPanic(int flag);

void setup() {
  
  Serial.begin(9600);
  
  //Set up temp sensors
  tempSensors.begin();

  //Set up ultrasonic pins
  pinMode(SONIC_TRIG, OUTPUT);
  pinMode(SONIC_ECHO, INPUT);

  //Set up button pin
  pinMode(BUTTONS, INPUT);

  //Set up gps
  gps_serial.begin(9600);
}

void loop() {
  
  float temp = 0.0;
  int sonic_distance = 0;
  int button_pressed = 0;

  int alertFlag = 0;
  
  //get temperature
  temp = getTemp();
  
  //get distance
  sonic_distance = getSonicDistance();
  
  //get buttons
  button_pressed = getButtons();

  //get gps data
  getGPS();
  
  if (button_pressed == 1 || sonic_distance < 45) { childInCar = '1'; }
  else { childInCar = '0'; }
  
  receive_buffer = millis() + RECEIVE;
  transmit_buffer = millis() + TRANSMIT;
  alertFlag = radio_receive();
  if (alertFlag == 0) {

    dontPanic(alertFlag);
    radio_transmit();
  }
  
  else {
    
    while (alertFlag == 1) {
      
      if (button_pressed == 1 || sonic_distance < 45) {
        
        panic(1, temp);
        
        //get temperature
        temp = getTemp();
  
        //get distance
        sonic_distance = getSonicDistance();
  
        //get buttons
        button_pressed = getButtons();

        //get gps data
        getGPS();
        
        receive_buffer = millis() + ((RECEIVE / 2) / 2) / 2;
        alertFlag = radio_receive();
      } else {
      
        alertFlag = 0;
        childInCar = '0';
        dontPanic(alertFlag);
      }
    }
  }
}

int radio_receive() {
  
  char received[1] = "";

  //Set up radio for receiving
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.openReadingPipe(0, address);
  radio.startListening();

  //Receive
  while(millis() < receive_buffer) {

    if (radio.available()) { radio.read(&received, sizeof(received)); }
  }
  if (received[0] != '1') { return 1; }
  return 0;
}

void radio_transmit() {
  
  //Set up radio for transmitting
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.openWritingPipe(address);
  radio.stopListening();
  
  //Transmit
  while(millis() < transmit_buffer) {
  
    char toSend[2];
    toSend[0] = '1';
    toSend[1] = childInCar;
    radio.write(&toSend, sizeof(toSend));
  }
}

float getTemp() {

  tempSensors.requestTemperatures();
  return tempSensors.getTempFByIndex(0);
}

int getSonicDistance() {
 
  long sonic_duration = 0;
  int sonic_distance = 0;
  
  //Clear SONIC_TRIG
  digitalWrite(SONIC_TRIG, LOW);
  delayMicroseconds(2);

  //Set SONIC_TRIG to High for 10 Microseconds
  digitalWrite(SONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(SONIC_TRIG, LOW);

  //read SONIC_ECHO
  sonic_duration = pulseIn(SONIC_ECHO, HIGH);

  //calculate distance 
  sonic_distance = sonic_duration * 0.034 / 2;

  return sonic_distance;
}

int getButtons() { return digitalRead(BUTTONS); }

void getGPS() {

  while(gps_serial.available()) { gps.encode(gps_serial.read()); }

  if (gps.location.isUpdated()) {
    double latTemp = gps.location.lat();
    double longTemp = gps.location.lng();
    if (latTemp != 0.0) { gps_lat = latTemp; }
    if (longTemp != 0.0) { gps_long = longTemp; }
  }
}

void panic(int flag, float temp) {

   Serial.print(flag);
   Serial.print(",");
   Serial.print(temp);
   Serial.print(",");
   Serial.print(gps_lat);
   Serial.print(",");
   Serial.print(gps_long);
   Serial.print(",");
   Serial.print(ID);
   Serial.print(",");
   Serial.println("");
}

void dontPanic(int flag) {

  Serial.print(flag);
  Serial.print(",");
  Serial.print(ID);
  Serial.print(",");
  Serial.println("");
}
