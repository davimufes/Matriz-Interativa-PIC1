# Trabalho-PIC – Display Interativo de Mesa
Display interativo embarcado que integra sensores, microcontrolador e matriz de LEDs endereçáveis, com múltiplos modos de operação definidos pela orientação espacial do dispositivo.

---

## 2. Descrição (Contextualização)
Este projeto consiste no desenvolvimento de um **display interativo de mesa**, concebido como trabalho principal da disciplina de **Projeto Integrado de Computação (PIC)**, do curso de **Engenharia de Computação**.

O objetivo do código é integrar hardware e software em um sistema embarcado completo, utilizando sensores, microcontrolador, sistema de alimentação por bateria e uma interface visual baseada em uma **matriz de LEDs endereçáveis**.  
O sistema apresenta comportamento dinâmico, alterando seus modos de operação de acordo com a orientação espacial detectada por um acelerômetro.

---

## 3. Estrutura do Projeto
A estrutura do projeto é composta pelos seguintes elementos:

- `main.ino`  
  Arquivo principal do Arduino contendo:
  - Inicialização dos periféricos
  - Leitura dos sensores
  - Lógica de controle dos modos de operação
  - Controle da matriz de LEDs

- `hardware/`
  Esquemático de circuito para controle e alimentação de matriz de LEDs WS2812B com Arduino UNO R3, RTC DS3231M, MPU-6050 e alimentação por bateria Li-ion, desenvolvido no KiCad.
 ## Imagem do Projeto

[![Display Interativo](PIC1-11-46_page-0001.jpg)](PIC1-11-46_page-0001.jpg)
  
- `docs/` *(opcional)*  
  Documentação complementar do projeto.

---

## 4. Como Compilar ou Executar
Este projeto foi desenvolvido para a plataforma **Arduino**.

### Passos para execução:
1. Abrir a **Arduino IDE**
2. Selecionar a placa **Arduino Uno**
3. Selecionar a porta USB correspondente
4. Abrir o arquivo `main.ino`
5. Compilar e realizar o upload do código para a placa

### Bibliotecas Utilizadas:
- Bibliotecas padrão da Arduino IDE
- Biblioteca para controle da matriz WS2812B
- Biblioteca para comunicação I²C
- Biblioteca para o sensor MPU-6050
- Biblioteca para módulo Real-Time Clock (RTC)

---

## 5. Como Usar
O funcionamento do sistema depende da **orientação espacial** do dispositivo:

- O Arduino realiza a leitura contínua do acelerômetro **MPU-6050** via comunicação I²C.
- A orientação detectada define automaticamente o modo de operação ativo.
- Cada modo permanece ativo até que uma mudança de posição seja identificada.

### Modos de Operação:
- **Modo Relógio:**  
  Exibe o horário atual utilizando um módulo **RTC**, garantindo precisão mesmo sem conexão à internet.
- **Modo Temperatura:**  
  Exibe a temperatura ambiente medida pelo sensor **LM35**, em graus Celsius.
- **Modo Luminária:**  
  A matriz de LEDs é acionada com iluminação azul que se altera de forma gradual.
- **Modo Jogo:**  
  Um jogo interativo em que o usuário controla uma “bola” na matriz de LEDs por meio da inclinação do dispositivo, simulando o efeito da gravidade.

---

## 6. Requisitos

### Software
- Arduino IDE
- Linguagem C/C++ (padrão Arduino)

### Hardware
- 1 × Arduino Uno
- 1 × Display de LED endereçável 16x16 WS2812B
- 1 × Módulo Real-Time Clock (RTC)
- 1 × Pacote de células 3,7 V – 18 Ah
- 1 × Unidade de carregamento USB TP4056
- 1 × Conversor CC 3,7 V – 12 V
- 1 × Bateria 3 V
- Carcaça confeccionada em material apropriado
- Painel de acrílico

---

## 7. Observações Finais
### Limitações
- Dependência da orientação correta para troca de modos
- Sensibilidade do acelerômetro a vibrações externas

### Decisões de Projeto
- Uso da orientação espacial como forma principal de interação
- Centralização da lógica de controle no microcontrolador
- Interface visual baseada exclusivamente na matriz de LEDs

### Possíveis Melhorias
- Inclusão de botões para seleção manual de modos
- Implementação de animações mais complexas na matriz de LEDs
- Otimização do consumo energético

---

## 8. Autoria
Projeto desenvolvido por:  
**Davi**, **Heitor** e **João Gabriel**

Disciplina: Projeto Integrado de Computação  
Curso: Engenharia de Computação
