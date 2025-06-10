
# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: **Ryan Micael Benício dos Santos**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, 09 de Junho de 2025

---

# Sintetizador de Áudio com a BitDogLab

## 1. Introdução

Este projeto é um "sintetizador" de áudio desenvolvido para a placa BitDogLab. O objetivo é explorar e implementar os conceitos de captura, armazenamento, processamento e reprodução de áudio digital em um sistema embarcado. O sistema grava um som de curta duração através de um microfone e o reproduz utilizando buzzers, com controle total via botões e feedback visual por LEDs e um display OLED.

## 2. Funcionalidades

**Gravação de Áudio**: O dispositivo utiliza o microfone para capturar um sinal de áudio analógico, que é digitalizado pelo ADC do microcontrolador e armazenado em um buffer na memória.

**Reprodução Sonora**: O áudio gravado é reproduzido através de dois buzzers passivos. A reprodução utiliza a técnica de Modulação por Largura de Pulso (PWM) para recriar a onda sonora. Para um efeito mais rico, um leve efeito de chorus é aplicado a um dos buzzers.

**Controle Interativo**: As funções de gravação e reprodução são controladas de forma independente por dois botões dedicados na placa.

**Feedback Visual (LED)**: Um LED RGB fornece o status do sistema em tempo real: a luz vermelha acende durante a gravação, e a verde acende durante a reprodução.

**Display OLED**: Uma tela OLED SSD1306 exibe mensagens de status, informando o usuário quando o dispositivo está gravando, uma funcionalidade expandida a partir da sugestão de visualização de forma de onda.

## 3. Como Usar

1.  **Ligar**: Conecte a placa BitDogLab a uma fonte de energia. Uma mensagem de boas-vindas aparecerá no display.
2.  **Gravar**: Pressione o **Botão A**. O LED vermelho acenderá e o display mostrará "Gravando...". Fale ou faça um som perto do microfone durante o tempo de gravação pré-definido. Ao final, o LED e a mensagem do display se apagarão.
3.  **Reproduzir**: Pressione o **Botão B**. O LED verde acenderá e o som gravado será reproduzido pelos buzzers.

## 4. Conceitos Técnicos Aplicados

O desenvolvimento deste projeto aborda conhecimentos fundamentais de eletrônica, programação e processamento de sinais:

**Conversão Analógico-Digital (ADC)**: Transformação do sinal sonoro do microfone em dados digitais que o microcontrolador pode processar.

**Modulação por Largura de Pulso (PWM)**: Técnica utilizada para recriar sons a partir de sinais digitais. O *duty cycle* do PWM é variado para simular a amplitude da onda sonora.

**Taxa de Amostragem**: Conceito crucial que define a frequência com que o sinal analógico é lido para garantir uma captura fiel do áudio.

**Armazenamento em Buffer**: Utilização de um array na memória RAM para armazenar as amostras de áudio digitalizadas antes da reprodução.

**Controle de Periféricos**: Programação de GPIOs para leitura de botões, controle de LEDs e comunicação via I2C com o display OLED.

**Timers**: Uso de timers de hardware para garantir que a amostragem do áudio ocorra em intervalos precisos e consistentes, permitindo atingir a taxa de amostragem desejada.

---

## 📜 Licença
GNU GPL-3.0.

