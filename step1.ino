#include <IRremote.h>

const int trigPin = 9; //trig part of ultrasonic sensor
const int echoPin = 10; //echo part of ultrasonic sensor
const int ledR = 13; //red led
const int ledY = 12; //yellow led
const int ledG = 11; //green led
const int senIR = 2; //infrared sensor
const int buzzer = 6; // buzzer
int myPIN[4] = {
  1,
  2,
  3,
  4
};
int attempt[4] = {};
boolean correctPIN = false;
boolean triggerAlarm = false;
boolean triggerCountdown = false;
int counter = 0;
int countDown = 6;
long lastMilli = 0;
unsigned int lastTime[5] = {};
int bcount = 0;
unsigned long currentMillis = 0;

IRrecv irrecv(senIR);
decode_results results;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  currentMillis = millis();
  lastMilli = currentMillis;
}

void loop() {

  long duration, cm;
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);
  Serial.print(cm);
  Serial.print(" cm");
  Serial.println();

  if (cm > 200) {
    digitalWrite(ledR, HIGH);
  } else {
    digitalWrite(ledR, LOW);
  }

  if (cm < 150 && cm > 100) {
    digitalWrite(ledY, HIGH);
  } else {
    digitalWrite(ledY, LOW);
  }

  if (!correctPIN) {
    if (cm < 50 && triggerCountdown != true) {
      triggerCountdown = true;
    }
  }

  if (triggerCountdown) {
    currentMillis = millis();

    if (currentMillis - lastMilli >= 1000) {
      countDown--;
      lastMilli = currentMillis;
      if (countDown == 0) {
        triggerAlarm = true;
      }
    }

  }

  if (!correctPIN && !triggerAlarm) {
    Serial.println(countDown);
    Serial.println(currentMillis);

    if (counter < 5) {
      if (irrecv.decode( & results)) {
        Serial.println(results.value, HEX);
        switch (results.value) {
        case 0xFF6897:
          attempt[counter] = 0;
          break;
        case 0xFF30CF:
          attempt[counter] = 1;
          break;
        case 0xFF18E7:
          attempt[counter] = 2;
          break;
        case 0xFF7A85:
          attempt[counter] = 3;
          break;
        case 0xFF10EF:
          attempt[counter] = 4;
          break;
        case 0xFF38C7:
          attempt[counter] = 5;
          break;
        case 0xFF5AA5:
          attempt[counter] = 6;
          break;
        case 0xFF42BD:
          attempt[counter] = 7;
          break;
        case 0xFF4AB5:
          attempt[counter] = 8;
          break;
        case 0xFF52AD:
          attempt[counter] = 9;
          break;
        }
        counter++;
        irrecv.resume();
        Serial.println("done");
      }
    }
  }

  if (counter == 4) {
    checkPIN();
    Serial.println(correctPIN);
    Serial.println(triggerAlarm);
    counter++;
  }

  if (correctPIN) {
    digitalWrite(ledG, HIGH);
  }

  if (triggerAlarm) {
    digitalWrite(ledR, HIGH);
    pinMode(buzzer, OUTPUT);
    tone(buzzer, 1000);
    delay(2000);
    noTone(buzzer);
    delay(500);
  }

  delay(100);

}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

void checkPIN() {
  int trueCounter = 0;

  for (int i = 0; i < 4; i++) {
    if (myPIN[i] == attempt[i]) {
      trueCounter++;
    }
  }

  if (trueCounter == 4) {
    correctPIN = true;
    triggerCountdown = false;
    countDown = 5;
  }

  if (!correctPIN) {
    triggerAlarm = true;
  }

  for (int i = 0; i < 4; i++) {
    Serial.println(myPIN[i]);
    Serial.println(attempt[i]);
  }
}
