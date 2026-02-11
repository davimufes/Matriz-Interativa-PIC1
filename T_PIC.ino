// ========================================================================================
// PROJETO: Display Gravitacional (PIC 1 - UFES)
// DESCRIÇÃO: Matriz de LED interativa com Acelerômetro, Relógio e Termômetro.
// GRUPO: Davi, Heitor e João
// ========================================================================================

// ----------------------------------------------------------------------------------------
// 1. BIBLIOTECAS
// ----------------------------------------------------------------------------------------
#include <Wire.h>       // Comunicação I2C
#include <MPU6050.h>    // Sensor de Movimento
#include "RTClib.h"     // Relógio de Tempo Real
#include <FastLED.h>    // Controle da Matriz de LEDs

// ----------------------------------------------------------------------------------------
// 2. DEFINIÇÕES DE HARDWARE E CONSTANTES
// ----------------------------------------------------------------------------------------
#define LED_PIN     7   // Pino de dados dos LEDs
#define NUM_LEDS    256 // Matriz 16x16
#define BRIGHTNESS  20  // Brilho limitado para economia de energia

// ----------------------------------------------------------------------------------------
// 3. INSTÂNCIAS E VARIÁVEIS GLOBAIS
// ----------------------------------------------------------------------------------------
CRGB leds[NUM_LEDS];    // Array de cores dos LEDs
RTC_DS1307 rtc;         // Objeto do Relógio
MPU6050 mpu(0x69);      // Sensor MPU (Endereço 0x69)
char buffer[16][16];    // Grade virtual para desenho antes de renderizar

// Variáveis de Filtragem (Filtro Passa-Baixa)
float axFiltrado = 0, ayFiltrado = 0, azFiltrado = 0;
float alfa = 0.2;       // Fator de suavização (0.0 a 1.0)

// Estado do Sistema
bool bateuNaParede = false;
int nivelAtual = 0;     // Nível do Labirinto (0 a 4)
int modoGlobal = 0;     // 0:Jogo, 1:Relógio, 2:Data, 3:Temperatura
int bolaX = 1, bolaY = 1; 

// ----------------------------------------------------------------------------------------
// 4. MAPAS E FONTES (DADOS DE MEMÓRIA)
// ----------------------------------------------------------------------------------------

// Labirintos: Cada uint16_t representa uma linha de 16 bits (1=parede, 0=caminho)
const uint16_t mapas[5][16] = {
    { 0xFFFF, 0x8081, 0xBC8D, 0xBC0D, 0xBC8D, 0x8081, 0xBFBF, 0xA021, 0xAFEF, 0xA101, 0xEFFD, 0xA101, 0xBDFD, 0x8081, 0x8001, 0xFFFF },
    { 0xFFFF, 0x8181, 0xFBB7, 0x8105, 0xBFF5, 0xA015, 0xACC5, 0x8845, 0xDBB7, 0x8B81, 0xEBFF, 0x8801, 0xBFFD, 0x8001, 0x8001, 0xFFFF },
    { 0xFFFF, 0x9FFF, 0x8201, 0xBAF5, 0x8295, 0xBE9D, 0x8081, 0xFEFF, 0x8001, 0x9E85, 0x8285, 0x82BD, 0x82A5, 0x9EBD, 0x8001, 0xFFFF },
    { 0xFFFF, 0x9005, 0xC87D, 0xAAD5, 0xAE15, 0xA2D1, 0xA945, 0x8855, 0x8BA9, 0xAA11, 0xAA43, 0xA925, 0xA61D, 0x9085, 0x8891, 0xFFFF },
    { 0xFFFF, 0xAAAB, 0xAAAB, 0x8001, 0xDFFD, 0xD001, 0x96FD, 0x9803, 0x87ED, 0xF225, 0x8A25, 0xBC25, 0x8105, 0xDFFD, 0x8001, 0xFFFF }
};

// Fonte Numérica: Matriz 5x7 para os dígitos de 0 a 9
const uint8_t fonteCubo[10][7] = {
    {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1
    {0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F}, // 2
    {0x1F, 0x01, 0x01, 0x0F, 0x01, 0x01, 0x1F}, // 3
    {0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01}, // 4
    {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F}, // 5
    {0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F}, // 6
    {0x1F, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}, // 7
    {0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F}, // 8
    {0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F}  // 9
};

const uint8_t simboloCelsius[7] = { 0b01100011, 0b10010100, 0b10010100, 0b01100100, 0b00000100, 0b00000100, 0b00000011 };

// ----------------------------------------------------------------------------------------
// 5. FUNÇÕES AUXILIARES DE DESENHO E RENDERIZAÇÃO
// ----------------------------------------------------------------------------------------

// Converte coordenadas X/Y para o índice linear do FastLED (Lógica de zigue-zague)
int getIndex(int x, int y) {
    if (y % 2 == 0) return (y * 16) + (15 - x);
    else return (y * 16) + x;
}

// Limpa a matriz virtual preenchendo com pontos
void limparBuffer() {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) buffer[i][j] = '.'; 
    }
}

// Escreve um caractere no buffer com suporte a rotação (90, 180, 270 graus)
void escreverNoBuffer(int x, int y, char c, int rot) {
    int nx = x, ny = y;
    if (rot == 90) { nx = y; ny = 15 - x; }
    else if (rot == 180) { nx = 15 - x; ny = 15 - y; }
    else if (rot == 270) { nx = 15 - y; ny = x; }
    
    if (nx >= 0 && nx < 16 && ny >= 0 && ny < 16) buffer[ny][nx] = c;
}

// Processa a fonte e desenha o dígito no buffer
void desenharDigito(int d, int xOff, int yOff, int rot) {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 5; j++) {
            if ((fonteCubo[d][i] >> (4 - j)) & 1) escreverNoBuffer(yOff + j, xOff + i, '#', rot);
        }
    }
}

// Converte o buffer de caracteres em cores reais nos LEDs
void renderizar() {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            int idx = getIndex(j, i);
            if (buffer[i][j] == '#') {
                // Aplicação de Gradientes por Modo
                if (modoGlobal == 1)      leds[idx] = blend(CRGB(40,0,80), CRGB(0,180,255), i*17); 
                else if (modoGlobal == 2) leds[idx] = blend(CRGB(0,120,0), CRGB(200,200,0), i*17); 
                else if (modoGlobal == 3) leds[idx] = blend(CRGB(200,0,0), CRGB(255,140,0), i*17); 
                else                      leds[idx] = CRGB(0, 0, 150); 
            }
            else if (buffer[i][j] == 'S') leds[idx] = CRGB(0, 255, 0); // Segundos fixos em Verde
            else if (buffer[i][j] == 'O') leds[idx] = bateuNaParede ? CRGB(255, 255, 255) : CRGB(255, 0, 0);
            else if (buffer[i][j] == 'X') leds[idx] = CRGB(0, 255, 0); // Objetivo
            else leds[idx] = CRGB(0, 0, 0);
        }
    }
    FastLED.show();
}

// ----------------------------------------------------------------------------------------
// 6. FUNÇÕES DE CADA MODO DO CUBO
// ----------------------------------------------------------------------------------------

void modoRelogio() {
    DateTime now = rtc.now();
    limparBuffer(); 
    modoGlobal = 1; 

    // Desenha Horas e Minutos (Rotação 270 para visualização frontal)
    desenharDigito(now.hour() / 10, 1, 2, 270);
    desenharDigito(now.hour() % 10, 1, 9, 270);
    desenharDigito(now.minute() / 10, 9, 2, 270);
    desenharDigito(now.minute() % 10, 9, 9, 270);

    // Barra de segundos nas bordas
    int seg = now.second();
    for (int i = 0; i < 15; i++) {
        if (seg > i)      escreverNoBuffer(i, 0, 'S', 0);   
        if (seg > i + 15) escreverNoBuffer(15, i, 'S', 0);   
        if (seg > i + 30) escreverNoBuffer(15-i, 15, 'S', 0);  
        if (seg > i + 45) escreverNoBuffer(0, 15-i, 'S', 0);  
    }
    renderizar();
}

void modoData() {
    DateTime now = rtc.now();
    limparBuffer(); modoGlobal = 2;
    desenharDigito(now.day() / 10, 1, 2, 180);
    desenharDigito(now.day() % 10, 1, 9, 180);
    desenharDigito(now.month() / 10, 9, 2, 180);
    desenharDigito(now.month() % 10, 9, 9, 180);
    renderizar();
}

void modoTemperatura() {
    int tempInt = (int)(mpu.getTemperature() / 340.0 + 27); // Conversão Datasheet + Offset
    limparBuffer(); modoGlobal = 3;
    desenharDigito(tempInt / 10, 1, 2, 90);
    desenharDigito(tempInt % 10, 1, 9, 90);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 8; j++) {
            if ((simboloCelsius[i] >> (7 - j)) & 1) escreverNoBuffer(4 + j, 9 + i, '#', 90);
        }
    }
    renderizar();
}

void modoJogo(int16_t ay, int16_t ax) {
    limparBuffer(); modoGlobal = 0; bateuNaParede = false;
    
    // Desenha o mapa do labirinto
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if ((mapas[nivelAtual][i] >> (15 - j)) & 1) escreverNoBuffer(j, i, '#', 0);
        }
    }
    escreverNoBuffer(14, 14, 'X', 0); // Alvo final do nível
    
    // Lógica de Movimentação da Bolinha
    int px = bolaX, py = bolaY;
    if (ax > 4000) px = constrain(bolaX + 1, 0, 15);
    else if (ax < -4000) px = constrain(bolaX - 1, 0, 15);
    
    // Checa colisão em X
    if (!((mapas[nivelAtual][bolaY] >> (15 - px)) & 1)) bolaX = px; 
    else if (abs(ax) > 4000) bateuNaParede = true;

    if (ay > 4000) py = constrain(bolaY + 1, 0, 15);
    else if (ay < -4000) py = constrain(bolaY - 1, 0, 15);
    
    // Checa colisão em Y
    if (!((mapas[nivelAtual][py] >> (15 - bolaX)) & 1)) bolaY = py; 
    else if (abs(ay) > 4000) bateuNaParede = true;

    // Condição de Vitória (Troca de Nível)
    if (bolaX == 14 && bolaY == 14) {
        fill_solid(leds, NUM_LEDS, CRGB::Green); FastLED.show(); delay(500);
        bolaX = 1; bolaY = 1; nivelAtual = (nivelAtual + 1) % 5; 
    }
    escreverNoBuffer(bolaX, bolaY, 'O', 0); // Desenha a bolinha
    renderizar();
}

// ----------------------------------------------------------------------------------------
// 7. FUNÇÕES PADRÃO ARDUINO (SETUP E LOOP)
// ----------------------------------------------------------------------------------------

void setup() {
    Serial.begin(9600); 
    Wire.begin();
    
    // Configuração Inicial de LEDs
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    
    // Inicialização de Sensores
    rtc.begin();
    // rtc.adjust(DateTime(2026, 2, 10, 19, 8, 20)); // Use para acertar a hora se necessário
    
    mpu.initialize();
}

void loop() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Aplicação do Filtro Passa-Baixa para suavizar o movimento
    axFiltrado = (alfa * ax) + ((1.0 - alfa) * axFiltrado);
    ayFiltrado = (alfa * ay) + ((1.0 - alfa) * ayFiltrado);
    azFiltrado = (alfa * az) + ((1.0 - alfa) * azFiltrado);

    // Máquina de Estados baseada na inclinação do Cubo
    if (azFiltrado > 10000)        modoJogo(ax, ay);      // Face Topo
    else if (ayFiltrado < -10000)  modoRelogio();        // Face Direita
    else if (axFiltrado < -10000)  modoData();           // Face em Pé
    else if (ayFiltrado > 10000)   modoTemperatura();    // Face Esquerda
    else { 
        limparBuffer(); 
        renderizar(); 
    }
    
    // Delay variável: mais rápido no jogo para melhor resposta
    delay(azFiltrado > 10000 ? 30 : 250);
}
