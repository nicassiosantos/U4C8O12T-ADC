# Projeto de Controle de Joystick e Display OLED com Raspberry Pi Pico

Este projeto utiliza o microcontrolador Raspberry Pi Pico para criar um sistema interativo que combina a leitura de um joystick analógico, o controle de LEDs via PWM e a exibição de gráficos em um display OLED SSD1306. O código foi desenvolvido em C e faz uso das bibliotecas do SDK do Raspberry Pi Pico para manipulação de periféricos como GPIO, ADC, PWM, I2C e o display OLED.

## Objetivos

- Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.
- Utilizar o PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick.
- Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
- Aplicar o protocolo de comunicação I2C na integração com o display.

## Componentes Principais

### Joystick Analógico

O joystick possui dois eixos (X e Y) que fornecem sinais analógicos. Esses sinais são lidos pelo Raspberry Pi Pico através de seus conversores analógico-digitais (ADC).

Os valores lidos são usados para controlar:
- O brilho dos LEDs azul e vermelho via PWM.
- A posição de um quadrado no display OLED.

### LEDs

- **LED Azul (LED_B) e LED Vermelho (LED_R):** Controlados via PWM, o brilho desses LEDs varia conforme a posição do joystick.
- **LED Verde (LED_G):** Controlado digitalmente, pode ser ligado ou desligado através de um botão.

### Display OLED SSD1306

Um display OLED de 128x64 pixels é utilizado para exibir um quadrado que se move conforme a posição do joystick.

O display também pode ser limpo ou ter uma borda desenhada através de botões.

### Botões

- **Botão A:** Liga/desliga o PWM dos LEDs azul e vermelho.
- **Botão do Joystick:** Liga/desliga o LED verde e controla a exibição de uma borda no display OLED.

## Funcionalidades do Projeto

1. **Leitura do Joystick**
   O joystick fornece valores analógicos para os eixos X e Y, que são convertidos em valores digitais pelo ADC do Raspberry Pi Pico.

   Esses valores são mapeados para controlar:
   - O brilho dos LEDs azul e vermelho via PWM.
   - A posição de um quadrado no display OLED.

2. **Controle de LEDs**
   - **LEDs Azul e Vermelho:** O brilho é ajustado com base na posição do joystick. Quando o joystick está na posição central, os LEDs ficam apagados. Ao mover o joystick, o brilho aumenta proporcionalmente à distância da posição central.
   - **LED Verde:** Pode ser ligado ou desligado através do botão do joystick. Quando ligado, uma borda é desenhada no display OLED.

3. **Display OLED**
   Um quadrado de 8x8 pixels é desenhado no display OLED. A posição do quadrado é atualizada conforme a posição do joystick. O display pode ser limpo ou ter uma borda desenhada através do botão do joystick.

4. **Botões de Controle**
   - **Botão A:** Liga/desliga o PWM dos LEDs azul e vermelho. Quando desligado, os LEDs são apagados e os pinos são reconfigurados como saídas digitais.
   - **Botão do Joystick:** Liga/desliga o LED verde. Controla a exibição de uma borda no display OLED.

## Estrutura do Código

O projeto está organizado em dois arquivos principais:

### 1. `adc.c`

Contém a lógica principal do projeto, incluindo:
- Inicialização do hardware (GPIO, ADC, PWM, I2C, display OLED).
- Leitura dos valores analógicos do joystick.
- Controle do brilho dos LEDs via PWM.
- Tratamento de interrupções geradas pelos botões.
- Atualização da posição do quadrado no display OLED.

### 2. `ssd1306.c`

Contém as funções para controle do display OLED SSD1306, incluindo:
- Inicialização e configuração do display.
- Envio de comandos e dados via I2C.
- Funções para desenhar pixels, preencher a tela, desenhar retângulos e quadrados.

## Detalhamento do Código em `adc.c`

O arquivo `adc.c` é o núcleo do projeto, onde toda a lógica de controle e interação é implementada. Abaixo está uma explicação detalhada de cada parte do código:

### 1. Inclusão de Bibliotecas

O código começa incluindo as bibliotecas necessárias para manipulação de GPIO, PWM, ADC, I2C e controle do display OLED.

### 2. Definições dos Pinos

As definições dos pinos são feitas para facilitar a configuração e manutenção do código. Isso inclui os pinos para os eixos do joystick, LEDs, botões e conexões I2C para o display OLED.

### 3. Variáveis Globais e Flags

Variáveis globais são usadas para armazenar estados e configurações que são compartilhadas entre diferentes funções.

### 4. Inicialização do PWM

Uma função é responsável por configurar os pinos como saídas PWM e definir o valor de "wrap" (valor máximo do contador PWM). Essa função é usada para inicializar os LEDs azul e vermelho.

### 5. Tratamento de Interrupções

Uma função de interrupção é usada para tratar os eventos gerados pelos botões, implementando um mecanismo de debounce para evitar leituras falsas e controlando o estado do PWM e do LED verde.

### 6. Configuração Inicial

Uma função de configuração inicial é responsável por configurar todos os periféricos necessários, incluindo GPIO, ADC, I2C e o display OLED.

### 7. Atualização do Display OLED

Uma função é responsável por atualizar a posição de um quadrado no display OLED com base nos valores do joystick.

### 8. Leitura do Joystick e Controle do PWM

Outra função é responsável por ler os valores do joystick, calcular os ciclos de trabalho do PWM e atualizar a posição do quadrado no display OLED.

### 9. Função Principal

A função principal do programa configura o hardware, inicializa as interrupções e entra em um loop infinito onde os valores do joystick são lidos e processados.


### Pré-requisitos

#### Hardware:
- Raspberry Pi Pico.
- Joystick analógico.
- Display OLED SSD1306.
- LEDs (azul, vermelho, verde).
- Botões.
- Resistores e jumpers para conexões.

### Conexões do Hardware

#### Joystick:
- Eixo Y: Pino 26 (VRY)  
- Eixo X: Pino 27 (VRX)  
- Botão: Pino 22 (button_joystick)

##### LEDs:
- Azul: Pino 12 (LED_B)  
- Vermelho: Pino 13 (LED_R)  
- Verde: Pino 11 (LED_G)

#### Botões:
- Botão A: Pino 5 (button_A)  
- Botão do Joystick: Pino 22 (button_joystick)

#### Display OLED:
- SDA: Pino 14 (I2C_SDA)  
- SCL: Pino 15 (I2C_SCL)

### Considerações Finais

Este projeto é um exemplo prático de como integrar diferentes componentes eletrônicos com um microcontrolador. Ele pode ser expandido para incluir mais funcionalidades, como:
- Leitura de mais sensores
- Controle de mais dispositivos
- Implementação de uma interface gráfica mais complexa no display OLED






