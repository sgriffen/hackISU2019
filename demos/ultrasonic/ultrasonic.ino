//ultrasonic
#define SONIC_TRIG A2
#define SONIC_ECHO A3

int getSonicDistance();

void setup() {
  
  Serial.begin(9600);
  pinMode(SONIC_TRIG, OUTPUT);
  pinMode(SONIC_ECHO, INPUT);
}

void loop() {

  int d = getSonicDistance();
  Serial.print("Distance: ");
  Serial.println(d);
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
