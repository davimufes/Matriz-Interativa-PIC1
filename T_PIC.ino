#include <Wire.h>
#include <MPU6050.h>
#include "RTClib.h"

float axFiltrado = 0, ayFiltrado = 0, azFiltrado = 0;
float alfa = 0.2;

RTC_DS1307 rtc;
MPU6050 mpu(0x69); 
char buffer[16][16];

// --- Fontes ---
const uint8_t fonteCubo[10][7] = {
  {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F}, // 0
  {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1
  {0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F}, // 2
  {0x1F, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x1F}, // 3
  {0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01}, // 4
  {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F}, // 5
  {0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F}, // 6
  {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10}, // 7
  {0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F}, // 8
  {0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F}  // 9
};

const uint8_t simboloCelsius[7] = {
  0b01100011, 0b10010100, 0b10010100, 0b01100100, 0b00000100, 0b00000100, 0b00000011
};

const uint16_t labirinto[16] = {
  0xFFFF, 0x8001, 0x87F1, 0x8401, 0x847D, 0x8441, 0x8441, 0x8001, 
  0x8111, 0x8111, 0x8111, 0x8001, 0x8F11, 0x8111, 0x8001, 0xFFFF
};

int bolaX = 8, bolaY = 8;

// --- Sistema de Desenho com Suporte a Rotação ---

void limparBuffer() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) buffer[i][j] = '.'; 
  }
}

void escreverNoBuffer(int x, int y, char c, int rotacao) {
  int nx = x, ny = y;
  if (rotacao == 90) { nx = y; ny = 15 - x; }
  else if (rotacao == 180) { nx = 15 - x; ny = 15 - y; }
  else if (rotacao == 270) { nx = 15 - y; ny = x; }

  if (nx >= 0 && nx < 16 && ny >= 0 && ny < 16) {
    buffer[ny][nx] = c;
  }
}

void desenharDigito(int digito, int xOff, int yOff, int rot) {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 5; j++) {
      if ((fonteCubo[digito][i] >> (4 - j)) & 1) {
        escreverNoBuffer(yOff + j, xOff + i, '#', rot);
      }
    }
  }
}

void desenharCelsius(int xOff, int yOff, int rot) {
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 8; j++) {
      if ((simboloCelsius[i] >> (7 - j)) & 1) {
        escreverNoBuffer(yOff + j, xOff + i, '#', rot);
      }
    }
  }
}

void renderizar() {
  Serial.println("\n\n\n\n\n");
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      Serial.print(buffer[i][j]); Serial.print(" ");
    }
    Serial.println("|");
  }
}

void modoRelogio() {
  DateTime now = rtc.now();
  limparBuffer();
  desenharDigito(now.hour() / 10, 1, 2, 0);
  desenharDigito(now.hour() % 10, 1, 9, 0);
  desenharDigito(now.minute() / 10, 9, 2, 0);
  desenharDigito(now.minute() % 10, 9, 9, 0);
  renderizar();
}

void modoTemperatura() {
  int16_t tempRaw = mpu.getTemperature();
  int tempInt = (int)(tempRaw / 340.0 + 36.53);
  limparBuffer();
  desenharDigito(tempInt / 10, 1, 2, 270);
  desenharDigito(tempInt % 10, 1, 9, 270);
  desenharCelsius(9, 4, 270);
  renderizar();
}

void modoData() {
  DateTime now = rtc.now();
  limparBuffer();
  desenharDigito(now.day() / 10, 1, 2, 180);
  desenharDigito(now.day() % 10, 1, 9, 180);
  desenharDigito(now.month() / 10, 9, 2, 180);
  desenharDigito(now.month() % 10, 9, 9, 180);
  renderizar();
}

void modoJogo(int16_t ax, int16_t ay) {
  limparBuffer();
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      if ((labirinto[i] >> (15 - j)) & 1) escreverNoBuffer(j, i, '#', 0);
    }
  }
  
  int px = bolaX, py = bolaY;
  if (ax > 5000) px--; if (ax < -5000) px++;
  if (ay > 5000) py++; if (ay < -5000) py--;

  if (px >= 0 && px < 16 && py >= 0 && py < 16) {
    if (!((labirinto[px] >> (15 - py)) & 1)) { bolaX = px; bolaY = py; }
  }
  escreverNoBuffer(bolaY, bolaX, 'O', 0);
  escreverNoBuffer(14, 14, 'F', 0);
  renderizar();
}

void setup() {
  Serial.begin(9600); Wire.begin();
  rtc.begin(); mpu.initialize();
  if (!rtc.isrunning()) rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  axFiltrado = (alfa * ax) + ((1.0 - alfa) * axFiltrado);
  ayFiltrado = (alfa * ay) + ((1.0 - alfa) * ayFiltrado);
  azFiltrado = (alfa * az) + ((1.0 - alfa) * azFiltrado);

  if (azFiltrado < -10000) modoJogo(ax, ay);
  else if (axFiltrado > 10000) modoRelogio();
  else if (axFiltrado < -10000) modoData();
  else if (ayFiltrado > 10000) modoTemperatura();
  else { limparBuffer(); renderizar(); }
  
  delay((azFiltrado < -10000) ? 100 : 500);
}
