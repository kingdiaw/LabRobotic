#include <SoftwareSerial.h>

const int BUZZER = A0;
const int PB = 12;
const int M1_DIR = A1;
const int M1_EN = 5;
const int M2_DIR = A2;
const int M2_EN = 6;
const int M3_DIR = A3;
const int M3_EN = 9;
const int MCUTX = 3;
const int MCURX = 2;
const int LED = 13;

SoftwareSerial hc06(MCURX, MCUTX); // RX, TX

int arr_output[] = {BUZZER, M1_DIR, M1_EN, M2_DIR, M2_EN, M3_DIR, M3_EN};
byte arr_size = sizeof(arr_output) / sizeof(arr_output[0]);

void beep() {
  digitalWrite(BUZZER, HIGH); delay(150);
  digitalWrite(BUZZER, LOW); delay(100);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  hc06.begin(9600);
  delay(100);
  for (byte i = 0; i < arr_size; i++)
    pinMode(arr_output[i], OUTPUT);
  pinMode(PB, INPUT);
  beep();
  while (HIGH == digitalRead(PB));
  hc06.println("PB was press!");
}

void loop() {
  // put your main code here, to run repeatedly:
//  while (1) {
//    if (Serial.available())
//      hc06.write(Serial.read());
//    if (hc06.available())
//      Serial.write(hc06.read());
//  }
  analogWrite(M1_EN, 255);
  digitalWrite(M1_DIR, HIGH);
  delay(2000);
  analogWrite(M1_EN, 0);
  delay(300);
  analogWrite(M1_EN, 255);
  digitalWrite(M1_DIR, LOW);
  delay(2000);
  analogWrite(M1_EN, 0);
  delay(1000);

  analogWrite(M2_EN, 255);
  digitalWrite(M2_DIR, HIGH);
  delay(2000);
  analogWrite(M2_EN, 0);
  delay(300);
  analogWrite(M2_EN, 255);
  digitalWrite(M2_DIR, LOW);
  delay(2000);
  analogWrite(M2_EN, 0);
  delay(1000);

  analogWrite(M3_EN, 255);
  digitalWrite(M3_DIR, HIGH);
  delay(2000);
  analogWrite(M3_EN, 0);
  delay(300);
  analogWrite(M3_EN, 255);
  digitalWrite(M3_DIR, LOW);
  delay(2000);
  analogWrite(M3_EN, 0);
  delay(1000);
}
