# Trabalho-PIC
Este repositório contém a documentação do projeto desenvolvido na disciplina de Projeto Integrado de Computação I, no curso de Engenharia de Computação.

Grupo:
Davi, Heitor e João Gabriel

Descrição Geral do Projeto
- Um display interativo de mesa, que pode mostrar as horas em formato digital, temperatura ou servir como iluminação, sendo cada funcionalidade selecionada a partir da posição detectada por um acelerômetro. Além disso, o display também projeta um jogo que o usuário pode controlar usando o sensor de acelerômetro.

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

Lógica de Programação
O arduino recebe a leitura do MPU usando conexão I2C e identifica a posição a partir da aceleração da gravidade, cada posição chama uma função que é mantida até ocorrer variação na leitura do acelerômetro.
A função de relógio imprime o horário tendo como base um módulo Real Time Clock para poder manter a hora sem conexão com internet

