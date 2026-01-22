#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <RTClib.h>
#include <MPU6050.h>

#define LED_PIN 6
#define NUM_LEDS 256
#define LM35_PIN A0

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
RTC_DS3231 rtc;
MPU6050 mpu;

// Variáveis do jogo
int ballX = 1, ballY = 1;

void setup() {
  Wire.begin();
  Serial.begin(9600);

  strip.begin();
  strip.setBrightness(50);
  strip.show();

  rtc.begin();
  mpu.initialize();
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  if (abs(ax) < 5000 && abs(ay) < 5000) {
    showClock();
  } 
  else if (ax > 8000) {
    showTemperature();
  } 
  else if (ay > 8000) {
    showBlue();
  } 
  else if (az > 12000) {
    playGame(ax, ay);
  }

  delay(200);
}

//Relogio
void showClock() {
  strip.clear();
  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();

  strip.setPixelColor(hour, strip.Color(255, 0, 0));
  strip.setPixelColor(minute, strip.Color(0, 0, 255));
  strip.show();
}

//Temperatura
void showTemperature() {
  strip.clear();
  float temp = analogRead(LM35_PIN) * 5.0 / 1023.0 * 100.0;

  int leds = map(temp, 0, 50, 0, NUM_LEDS);
  for (int i = 0; i < leds; i++) {
    strip.setPixelColor(i, strip.Color(255, 100, 0));
  }
  strip.show();
}

//Luminaria
void showBlue() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 150));
  }
  strip.show();
}

//Jogo
void playGame(int ax, int ay) {
  strip.clear();

  if (ax > 3000) ballX++;
  if (ax < -3000) ballX--;
  if (ay > 3000) ballY++;
  if (ay < -3000) ballY--;

  ballX = constrain(ballX, 0, 15);
  ballY = constrain(ballY, 0, 15);

  int index = ballY * 16 + ballX;
  strip.setPixelColor(index, strip.Color(255, 255, 255));
  strip.show();
}