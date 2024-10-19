#include "rgb_lcd.h"

#define IDLE 0
#define SCARY 1
#define TERRIFY 3

const int pinButton = 4;
const int pinLed    = 3;

// Ping sensor
const int trigPin = 8;
const int echoPin = 9;

// running average
const int numReadings = 15;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int averageDistance = 0;

// PIR sensor
const int pirPin = 7;

// current mode based on sensor fusion
int currentMode = 0;

 rgb_lcd lcd;

double distance = 0;
int timeout = 0;
int lastPingTime = 0;
int pingDelta = 0;
char received = 0;
int byteCounter = 0;
int pirValue = 0;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;


void setup() {
  pinMode(pinButton, INPUT);
  pinMode(pinLed, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pirPin,INPUT);

// set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    // Print a message to the LCD.
    lcd.print("ScaryWindow setup");

   // Initialize the readings array with zeros
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
  Serial.begin(9600);
  // lcd.begin(16, 2);
  establishContact();  // send a byte to establish contact until receiver responds
}

void loop() {
    //lcd.clear();
  // show status on LCD
  if (currentMode == IDLE) {
    lcd.print("Idle");
    lcd.setRGB(0, 255, 0);
  } else if (currentMode == SCARY) {
    lcd.print("Scary");
    lcd.setRGB(255, 0, 0);
  } else if (currentMode == TERRIFY) { 
    lcd.print("Terrify");
  }

  /*
  if(digitalRead(pinButton))
  {
      // When the button is pressed, turn the LED on.
      digitalWrite(pinLed, HIGH);
      distance = 100;
  }
  else
  {
      // Otherwise, turn the LED off.
      digitalWrite(pinLed, LOW);
      distance = 500;
  }*/

  pirValue = digitalRead(pirPin);
  if (pirValue == HIGH) {
    digitalWrite(pinLed,HIGH);
    currentMode = SCARY;
  } else {
    digitalWrite(pinLed,LOW);
    currentMode = IDLE;
  }
  // Serial.println(currentMode);

  // Send a 5-microsecond pulse to the trigger pin
 
 /*
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the time it takes for the echo pin to receive the reflected sound wave
  long duration = pulseIn(echoPin, HIGH); // 20000L maybe?
  // Serial.println(duration);
  if (duration < 10){
    duration = 23300;
  }

  // Calculate the distance in centimeters using the speed of sound (343 m/s)
  distance = duration * 0.0343 / 2;

  // Subtract the previous reading and add the new reading to the total
  total = total - readings[readIndex];
  readings[readIndex] = distance;
  total = total + readings[readIndex];

  // Move to the next index in the readings array
  readIndex = (readIndex + 1) % numReadings;

  // Calculate the running average
  averageDistance = total / numReadings;

  lcd.clear();
  lcd.print(averageDistance);

  */
  // delay(100);  

  // if we get a valid byte, send update:
  if (Serial.available() > 0) {
    lastPingTime = millis();
    //Serial.write('ABB');
    // Serial.write(distance);
    // Serial.write('\n');

    Serial.println(currentMode);
    received = Serial.read();

    // print ping sensor values - not used
    /*
    Serial.println(averageDistance);
    received = Serial.read();
    lcd.setCursor(0, 1);
    lcd.print(pingDelta);
    lcd.print(" - ");
    lcd.print(byteCounter);
    lcd.setCursor(0, 0);
    */

    byteCounter++;
  } else {
    pingDelta = millis() - lastPingTime;
    if (pingDelta > 5000) {
     // establishContact();
    }
  }
}

void establishContact() {
  lcd.clear();
  lcd.print("not connected");

  while (Serial.available() <= 0) {
    Serial.print('*');  // send a *
    Serial.flush();
    delay(300);
  }

  lcd.clear();
  lcd.print("connected");
}
