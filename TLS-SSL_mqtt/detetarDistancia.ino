#include <Wire.h>
#define echoPin 32
#define trigPin 33

long duration, distance;

void setup() {
  // Start the serial communication:
  Serial.begin(115200);
  // Set up pin (e.g., the onboard LED pin) as an output:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delay(2);

  digitalWrite(trigPin, HIGH);
  delay(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  String disp = String(distance);

  Serial.print("Distancia: ");
  Serial.print(disp);
  Serial.println(" cm");
  delay(1000);
}