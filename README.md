# Matriz de LED Interativa
Repositório do trabalho da disciplina de PIC 1 do curso de Engenharia de Computação da Universidade Federal do Espírito Santo (UFES).
---

## Resumo
O projeto consiste em um dispositivo embarcado que utiliza uma matriz de LEDs endereçáveis para fornecer informações em tempo real e entretenimento interativo. O objetivo principal é explorar a integração de sensores inerciais com processamento de sinais para criar uma interface de usuário baseada inteiramente na orientação física do objeto. O dispositivo exibe dados de tempo, data, temperatura ambiente e um jogo de labirinto com física gravitacional simulada.

---

## Descrição
O dispositivo funciona à base de uma bateria de 3,7 V com 15 Ah, possuindo duas cargas principais: o display de LED RGB endereçável 16x16 e o circuito de controle.
A malha de potência da matriz de LED recebe a tensão de 3,7 V diretamente da bateria para otimizar a eficiência, enquanto o circuito de controle (malha lógica) é alimentado com 5 V por meio de um conversor step-up.
O sistema utiliza uma **Máquina de Estados Finita (FSM)** para alternar modos:
* Modo Jogo: Ativado na face superior. Labirinto com física de inclinação e 5 mapas com dificuldade crescente.
* Modo Relógio e Data: Sincronizados via RTC com gradientes de cores distintos.
* Modo Temperatura: Monitoramento térmico via sensor interno do MPU6050.
O processamento utiliza um **Filtro Passa-Baixa (DSP)** para suavizar as leituras do acelerômetro, garantindo estabilidade nas transições de estados e na movimentação da bolinha no jogo.

---

## Componentes e Materiais
* **Arduino Uno:** Microcontrolador principal.
* **Matriz de LED WS2812B 16x16:** Painel de 256 pixels endereçáveis.
* **Sensor MPU6050:** Acelerômetro e temperatura.
* **Módulo RTC DS1307:** Real-Time Clock para manutenção do tempo.
* **Bateria 3,7 V (15 Ah):** Fonte de energia primária, confeccionada com 5 células 18650 para duração prolongada
* **Conversor Boost HW-553:** Elevação de tensão para 5 V.
* **Protoboard e Jumpers:** Conexões da malha lógica.
* **Cabos e Conectores:** Estrutura da malha de potência.
* **Caixa de Acrílico:** Carcaça do dispositivo

---

## Pinout e Conexões

| Componente | Pino Arduino | Função | Protocolo |
| :--- | :---: | :--- | :---: |
| **Matriz de LED** | D7 | Sinal de Dados (DIN) | Digital |
| **Sensor MPU6050** | A4 / A5 | SDA - SCL | I2C |
| **Módulo RTC** | A4 / A5 | SDA - SCL  | I2C |
| **Sistema** | GND | Referência Comum | - |

### Desenho Esquemático do Circuito

[![Display Interativo](PIC1-11-46_page-0001.jpg)](PIC1-11-46_page-0001.jpg)

---

## Bibliotecas Utilizadas
* **FastLED:** Controle dos LEDs e gestão de limite de corrente.
* **MPU6050:** Interface com o acelerômetro e giroscópio.
* **RTClib (Adafruit):** Comunicação com o RTC.
* **Wire.h:** Comunicação I2C (Nativa).
* **avr/pgmspace.h:** Otimização de memória Flash (Nativa).

---

## Desafios

Durante o desenvolvimento em PIC 1, foram superados obstáculos críticos:
* Mau Funcionamento de Componentes: Uma matriz estragou por algum motivo que não sabemos, tivemos que comprar outra. 
* Falhas na Ligação do Circuito: Provavelmente fechamos curto pois queimamos o carregador e uma das células da bateria.
* Estabilidade de Sinal: Resolução de ruídos e mau contato e várias partes do circuito, inclusive em um momento o pino d6 parou de funcionar, uma solução para instabilidade teria sido usar uma placa de circuito impresso porém não conseguimos nos organizar para desenvolvê-la.
* Gestão de Energia: Implementação de limite de potência via software (setMaxPower) para evitar o desligamento térmico do conversor step-up em picos de brilho.
* Eficiência de Memória: Uso de bitmasks para armazenar os mapas dos labirintos, economizando memória RAM do Arduino Uno.

---

## Créditos

Ao longo de muitas horas de desenvolvimento, contamos com a ajuda de diversas pessoas. Destacamos aqui o técnico Ronney, que uniu as células que compramos em uma bateria, e nos ajudou quando estragamos uma das células; o técnico Rasley que nos auxiliou com solda e resolução de problemas de projeto de hardware; os monitores Gustavo e Gabriel e o professor Jadir da disciplina de PIC 1.

---

## Grupo
* Davi Milanez de Almeida
* Heitor Valim Bianconi
* João Gabriel Rosa Pereira
