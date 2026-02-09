#include <Wire.h>
#include <MPU6050.h>
#include "RTClib.h"
#include <FastLED.h>

#define LED_PIN     7
#define NUM_LEDS    256 
#define BRIGHTNESS  20

CRGB leds[NUM_LEDS];
RTC_DS1307 rtc;
MPU6050 mpu(0x69); 
char buffer[16][16];

float axFiltrado = 0, ayFiltrado = 0, azFiltrado = 0;
float alfa = 0.2;
bool modoAtualRelogio = false; 
bool bateuNaParede = false; // Flag para o efeito de vibração

// --- Mapas e Fontes permanecem os mesmos ---
int nivelAtual = 0; 
const uint16_t mapas[5][16] = {
  { 0xFFFF, 0x8081, 0xBC8D, 0xBC8D, 0xBC8D, 0x8081, 0xBFBF, 0xA021, 0xAFEF, 0xA101, 0xAFFD, 0xA101, 0xBDFD, 0x8081, 0x8081, 0xFFFF },
  { 0xFFFF, 0x8001, 0x8FE1, 0x8021, 0x8EE1, 0x8221, 0x8EE1, 0x8221, 0x8EE1, 0x8221, 0x8EE1, 0x8221, 0x8021, 0x8FE1, 0x8001, 0xFFFF },
  { 0xFFFF, 0x9001, 0x9FF1, 0x9001, 0x90F1, 0x9001, 0x9FF1, 0x9001, 0x90F1, 0x8001, 0x9FF1, 0x9001, 0x90F1, 0x9001, 0x9001, 0xFFFF },
  { 0xE003, 0x9005, 0xC87D, 0xAAD5, 0xAE15, 0xA2D1, 0xA945, 0x8855, 0x8BA9, 0xAA11, 0xAA43, 0xA925, 0xA61D, 0x9085, 0x8891, 0xFFFF },
  { 0xFFFF, 0x8001, 0xABB1, 0x8001, 0x8201, 0xBEFB, 0x8001, 0x8BD1, 0x8BD1, 0x8001, 0xA001, 0xBF7B, 0x8001, 0x8E01, 0x8001, 0xFFFF }
};

int bolaX = 1, bolaY = 1; 

const uint8_t fonteCubo[10][7] = {
  {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F}, {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
  {0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F}, {0x1F, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x1F},
  {0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01}, {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F},
  {0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F}, {0x0F, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x0F},
  {0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F}, {0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F}
};

const uint8_t simboloCelsius[7] = { 0b01100011, 0b10010100, 0b10010100, 0b01100100, 0b00000100, 0b00000100, 0b00000011 };

// --- Funções Auxiliares ---
int getIndex(int x, int y) {
  if (y % 2 == 0) return (y * 16) + (15 - x);
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

void desenharDigito(int d, int xOff, int yOff, int rot) {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 5; j++) {
      if ((fonteCubo[d][i] >> (4 - j)) & 1) escreverNoBuffer(yOff + j, xOff + i, '#', rot);
    }
  }
}

void renderizar() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      int idx = getIndex(j, i);
      if (buffer[i][j] == '#') {
        if (modoAtualRelogio) leds[idx] = blend(CRGB(40,0,80), CRGB(0,180,255), i*17); 
        else leds[idx] = CRGB(0, 0, 150); 
      }
      else if (buffer[i][j] == 'O') {
        // Efeito de piscar ao bater na parede
        leds[idx] = bateuNaParede ? CRGB(255, 200, 200) : CRGB(200, 0, 0);
      }
      else if (buffer[i][j] == 'X') leds[idx] = CRGB(0, 200, 0);
      else leds[idx] = CRGB(0, 0, 0);
    }
  }
  FastLED.show();
}

// --- Modos de Operação ---
void modoRelogio() {
  DateTime now = rtc.now();
  limparBuffer(); modoAtualRelogio = true; 
  desenharDigito(now.hour() / 10, 1, 2, 0);
  desenharDigito(now.hour() % 10, 1, 9, 0);
  desenharDigito(now.minute() / 10, 9, 2, 0);
  desenharDigito(now.minute() % 10, 9, 9, 0);
  renderizar();
}

void modoJogo(int16_t ax, int16_t ay) {
  limparBuffer();
  modoAtualRelogio = false;
  bateuNaParede = false; // Reset da flag de colisão
  
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      if ((mapas[nivelAtual][i] >> (15 - j)) & 1) escreverNoBuffer(j, i, '#', 0);
    }
  }
  
  escreverNoBuffer(14, 14, 'X', 0);

  int px = bolaX, py = bolaY;
  bool tentouMover = false;

  // Eixo X
  if (ax > 4000) { px = bolaX + 1; tentouMover = true; }
  else if (ax < -4000) { px = bolaX - 1; tentouMover = true; }
  
  if (tentouMover) {
    if (!((mapas[nivelAtual][bolaY] >> (15 - constrain(px, 0, 15))) & 1)) bolaX = constrain(px, 0, 15);
    else bateuNaParede = true;
  }

  // Eixo Y
  tentouMover = false;
  if (ay > 4000) { py = bolaY + 1; tentouMover = true; }
  else if (ay < -4000) { py = bolaY - 1; tentouMover = true; }

  if (tentouMover) {
    if (!((mapas[nivelAtual][constrain(py, 0, 15)] >> (15 - bolaX)) & 1)) bolaY = constrain(py, 0, 15);
    else bateuNaParede = true;
  }

  // Vitória
  if (bolaX == 14 && bolaY == 14) {
    fill_solid(leds, NUM_LEDS, CRGB::Green); FastLED.show(); delay(500);
    bolaX = 1; bolaY = 1; nivelAtual = (nivelAtual + 1) % 5; 
  }
  
  escreverNoBuffer(bolaX, bolaY, 'O', 0);
  renderizar();
}

// ... (modoData e modoTemperatura continuam iguais) ...

void setup() {
  Serial.begin(9600); Wire.begin();
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  rtc.begin(); mpu.initialize();
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  axFiltrado = (alfa * ax) + ((1.0 - alfa) * axFiltrado);
  ayFiltrado = (alfa * ay) + ((1.0 - alfa) * ayFiltrado);
  azFiltrado = (alfa * az) + ((1.0 - alfa) * azFiltrado);

  if (azFiltrado > 10000)        modoJogo(ax, ay);
  else if (axFiltrado > 10000)   modoRelogio();
  // ... outros modos ...
  else { limparBuffer(); renderizar(); }
  
  // Delay reduzido para o Jogo ser mais responsivo
  delay(azFiltrado > 10000 ? 30 : 250);
}
