#include <Wire.h>
#include <MPU6050.h>
#include "RTClib.h"
#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    256 
#define BRIGHTNESS  40 

CRGB leds[NUM_LEDS];
RTC_DS1307 rtc;
MPU6050 mpu(0x69); 
char buffer[16][16];

float axFiltrado = 0, ayFiltrado = 0, azFiltrado = 0;
float alfa = 0.2;
bool modoAtualRelogio = false; // Flag para aplicar o gradiente

// --- Cores do Gradiente ---
CRGB corRoxo = CRGB(40, 0, 80);    // Roxo escuro
CRGB corAzul = CRGB(0, 180, 255);  // Azul claro

// --- Labirinto (Bitmask 16x16) ---
const uint16_t labirinto[16] = {
  0xFFFF, 0x8001, 0x8FBD, 0x8801, 0x88BD, 0x8F81, 0x88BD, 0x8001,
  0xBFFB, 0x8111, 0x8EEF, 0x8111, 0x8E8F, 0x8111, 0x8001, 0xFFFF
};

int bolaX = 1, bolaY = 1; // Começa no canto oposto ao (14,14)

// --- Fontes e Desenho ---
const uint8_t fonteCubo[10][7] = {
  {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F}, {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
  {0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F}, {0x1F, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x1F},
  {0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01}, {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F},
  {0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F}, {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10},
  {0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F}, {0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F}
};

const uint8_t simboloCelsius[7] = {
  0b01100011, 0b10010100, 0b10010100, 0b01100100, 0b00000100, 0b00000100, 0b00000011
};

int getIndex(int x, int y) {
  if (y % 2 == 0) return (y * 16) + (15 - x); // Correção do espelhamento Y
  else return (y * 16) + x;
}

void limparBuffer() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) buffer[i][j] = '.'; 
  }
}

void escreverNoBuffer(int x, int y, char c, int rot) {
  int nx = x, ny = y;
  if (rot == 90) { nx = y; ny = 15 - x; }
  else if (rot == 180) { nx = 15 - x; ny = 15 - y; }
  else if (rot == 270) { nx = 15 - y; ny = x; }
  if (nx >= 0 && nx < 16 && ny >= 0 && ny < 16) buffer[ny][nx] = c;
}

void desenharDigito(int digito, int xOff, int yOff, int rot) {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 5; j++) {
      if ((fonteCubo[digito][i] >> (4 - j)) & 1) escreverNoBuffer(yOff + j, xOff + i, '#', rot);
    }
  }
}

void desenharCelsius(int xOff, int yOff, int rot) {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 8; j++) {
      if ((simboloCelsius[i] >> (7 - j)) & 1) escreverNoBuffer(yOff + j, xOff + i, '#', rot);
    }
  }
}

// --- Renderização com Gradiente Linear ---
void renderizar() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      int idx = getIndex(j, i);
      if (buffer[i][j] == '#') {
        if (modoAtualRelogio) {
          // Interpolação de cor baseada na linha (i)
          leds[idx] = blend(corRoxo, corAzul, i * 17); 
        } else {
          leds[idx] = CRGB(0, 0, 150); // Azul padrão para outros modos
        }
      }
      else if (buffer[i][j] == 'O') leds[idx] = CRGB(200, 0, 0);
      else if (buffer[i][j] == 'X') leds[idx] = CRGB(0, 200, 0);
      else leds[idx] = CRGB(0, 0, 0);
    }
  }
  FastLED.show();
}

// --- Modos de Operação ---
void modoRelogio() {
  DateTime now = rtc.now();
  limparBuffer();
  modoAtualRelogio = true; // Ativa gradiente
  
  desenharDigito(now.hour() / 10, 1, 2, 0);
  desenharDigito(now.hour() % 10, 1, 9, 0);
  desenharDigito(now.minute() / 10, 9, 2, 0);
  desenharDigito(now.minute() % 10, 9, 9, 0);
  renderizar();
}

void modoData() {
  DateTime now = rtc.now();
  limparBuffer();
  modoAtualRelogio = false;
  desenharDigito(now.day() / 10, 1, 2, 180);
  desenharDigito(now.day() % 10, 1, 9, 180);
  desenharDigito(now.month() / 10, 9, 2, 180);
  desenharDigito(now.month() % 10, 9, 9, 180);
  renderizar();
}

void modoTemperatura() {
  int tempInt = (int)(mpu.getTemperature() / 340.0 + 36.53);
  limparBuffer();
  modoAtualRelogio = false;
  desenharDigito(tempInt / 10, 1, 2, 270);
  desenharDigito(tempInt % 10, 1, 9, 270);
  desenharCelsius(9, 4, 270);
  renderizar();
}

void modoJogo(int16_t ax, int16_t ay) {
  limparBuffer();
  modoAtualRelogio = false;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      if ((labirinto[i] >> (15 - j)) & 1) escreverNoBuffer(j, i, '#', 0);
    }
  }
  escreverNoBuffer(14, 14, 'X', 0); // Final no canto inferior direito

  int px = bolaX, py = bolaY;
  if (ax > 8000) px = constrain(bolaX + 1, 0, 15);
  else if (ax < -8000) px = constrain(bolaX - 1, 0, 15);
  if (!((labirinto[bolaY] >> (15 - px)) & 1)) bolaX = px;

  if (ay > 8000) py = constrain(bolaY + 1, 0, 15);
  else if (ay < -8000) py = constrain(bolaY - 1, 0, 15);
  if (!((labirinto[py] >> (15 - bolaX)) & 1)) bolaY = py;

  if (bolaX == 14 && bolaY == 14) {
    fill_solid(leds, NUM_LEDS, CRGB::Green); FastLED.show(); delay(500);
    bolaX = 1; bolaY = 1; // Reseta para o início oposto
  }
  escreverNoBuffer(bolaX, bolaY, 'O', 0);
  renderizar();
}

void setup() {
  Serial.begin(9600); Wire.begin();
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  rtc.begin(); mpu.initialize();
  if (!rtc.isrunning()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  axFiltrado = (alfa * ax) + ((1.0 - alfa) * axFiltrado);
  ayFiltrado = (alfa * ay) + ((1.0 - alfa) * ayFiltrado);
  azFiltrado = (alfa * az) + ((1.0 - alfa) * azFiltrado);

  if (azFiltrado < -10000)      modoJogo(ax, ay);
  else if (axFiltrado > 10000)  modoRelogio();
  else if (axFiltrado < -10000) modoData();
  else if (ayFiltrado > 10000)  modoTemperatura();
  else { limparBuffer(); renderizar(); }
  
  delay(azFiltrado < -12000 ? 50 : 300);
}
