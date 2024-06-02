#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Arduino.h>
#include <Wire.h>

#define pb 4
#define ledB 5
#define ledG 6
#define ledR 10
#define ledW 9
#define phto A0
const int cnty1Pin = 11;
const int cnty2Pin = 10;

Servo cnty1;
Servo cnty2;

int pos = 0;
int cek = -1;
bool benda = 1;

struct rangeColor
{
  int blue, red, green, white;
};

rangeColor colorRange;

struct rgb
{
  int red;
  int green;
  int blue;
};

rgb readValue;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void resetSystem()
{
  digitalWrite(ledR, LOW);
  digitalWrite(ledW, LOW);
  digitalWrite(ledG, LOW);
  digitalWrite(ledB, LOW);
  cnty1.write(0);
  cnty2.write(0);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MEMULAI");
  lcd.clear();
  delay(700);
}

void detectedColor(String color, int degree, int ledPin)
{
  lcd.setCursor(0, 0);
  lcd.print("Warna : ");
  lcd.print(color);
  delay(500);
  if (degree >= 0)
  {
    cnty1.write(degree);
    cnty2.write(degree);
  }
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
}

void setup()
{
  colorRange.blue = 20;
  colorRange.red = 40;
  colorRange.green = 60;
  colorRange.white = 80;

  Serial.begin(9600);
  digitalWrite(ledW, HIGH);
  pinMode(pb, INPUT_PULLUP);
  pinMode(ledR, OUTPUT);
  pinMode(ledW, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  cnty1.attach(7);
  cnty2.attach(8);
  cnty1.write(0);
  cnty2.write(0);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("READY!!");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("2");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("3");
}

void loop()
{
  lcd.clear();

  float val = analogRead(phto);
  Serial.print("nilai phto = ");
  Serial.println(val);

  lcd.setCursor(0, 1);
  lcd.print("Value : ");
  lcd.print(val);

  if (colorRange.white <= val)
  {
    detectedColor("White", 45, 13);
  }
  else if (colorRange.green <= val && val < colorRange.white)
  {
    detectedColor("Green", 90, ledG);
  }
  else if (colorRange.red <= val && val < colorRange.green)
  {
    detectedColor("Red", 145, ledR);
  }
  else if (colorRange.blue <= val && val < colorRange.red)
  {
    detectedColor("Blue", 45, ledB);
  }
  else
  {
    detectedColor("No Box", -1, 13);
  }

  delay(500);
}