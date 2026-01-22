# Trabalho-PIC

Descrição Geral
Este projeto consiste no desenvolvimento de um display interativo de mesa, concebido como trabalho principal da disciplina de Projeto Integrado de Computação, do curso de Engenharia de Computação.
O sistema integra hardware e software, utilizando sensores, microcontrolador, sistema de alimentação por bateria e uma interface visual baseada em uma matriz de LEDs endereçáveis, resultando em um dispositivo embarcado interativo e multifuncional.

O display é capaz de alterar dinamicamente seu comportamento de acordo com a orientação espacial, detectada por um acelerômetro, oferecendo diferentes modos de operação, como exibição de horário, temperatura ambiente, iluminação decorativa e um jogo interativo controlado por inclinação.

Grupo
Davi, Heitor e João Gabriel

Arquitetura de Hardware
- *inserir imagem*
Componentes utilizados:
- 1x Arduino Uno
- 1x Display de LED endereçável 16x16 Ws2812b
- 1x Pacote de células com 3,7v e 18Ah
- 1x Unidade de carregamento usb tp4056
- 1x Conversor CC 3,7v-12v
- 1x Sensor de temperatura LM35
- 1x Sensor acelerômetro MPU-6050
- 1x Módulo Real-Time Clock
- 1x Bateria 3v
- Carcaça feita em xxxxxxx
- Painel de acrílico

Lógica de Funcionamento
O Arduino realiza a leitura contínua do sensor MPU6050 por meio da comunicação I²C, utilizando a aceleração da gravidade para identificar a orientação do dispositivo. Cada orientação corresponde a um modo de funcionamento específico, que permanece ativo até que seja detectada uma mudança na posição.
No modo relógio, o sistema exibe o horário atual com base em um módulo Real Time Clock (RTC), garantindo a manutenção da hora independentemente de conexão com a internet.
O modo temperatura utiliza o sensor LM35 para medir a temperatura ambiente, convertendo o sinal analógico em valores expressos em graus Celsius.
No modo luminária, a matriz de LEDs é acionada em uma tonalidade azul uniforme.
No modo jogo, o usuário controla uma “bola” exibida na matriz de LEDs por meio da inclinação do dispositivo para escapar de um labirinto, utilizando o acelerômetro para simular o efeito da gravidade.
