# Componentes — Sertão Bee

Este documento descreve cada componente do projeto: o que faz, por que foi escolhido, parâmetros de operação relevantes e cuidados de uso.

> Para ligações pino a pino, ver [`ligacoes.md`](ligacoes.md).
> Para o papel de cada componente no fluxo de dados, ver [`arquitetura.md`](arquitetura.md).

---

## Lista de Materiais (BOM)

| # | Item | Quantidade | Onde é usado |
|---|---|---|---|
| 1 | ESP32 WROOM DevKit (USB-C) | 2 | Transmissor e receptor |
| 2 | Módulo LoRa SX1278 433 MHz | 2 | Transmissor e receptor |
| 3 | Antena helicoidal 433 MHz | 2 | Soldada em cada LoRa |
| 4 | Sensor AHT10 (I²C) | 1 | Transmissor |
| 5 | Célula de carga 50 kg | 1 | Transmissor |
| 6 | Módulo HX711 | 1 | Transmissor |
| 7 | Display OLED SSD1306 0,96" (I²C) | 1 | Receptor |
| 8 | Protoboard 400 pontos | 2 | Montagem |
| 9 | Jumpers macho-macho e macho-fêmea | ~30 | Ligações |
| 10 | Cabos USB-C | 2 | Alimentação / programação |

**Custo estimado do kit completo:** R$ 350 a R$ 500, considerando aquisição no mercado brasileiro.

---

## 1. ESP32 WROOM DevKit

**O que é.** Microcontrolador de 32 bits com Wi-Fi e Bluetooth integrados, dois núcleos Tensilica LX6 a até 240 MHz, 520 KB de SRAM e 4 MB de flash (modelo padrão).

**Por que foi escolhido.**
- Custo acessível e disponibilidade no mercado brasileiro.
- Bibliotecas maduras para LoRa, I²C e HX711.
- Margem de processamento confortável para o trabalho exigido pelo MVP.
- Permite expansão futura: o mesmo hardware suporta Wi-Fi local (Fase 3), BLE e armazenamento em microSD sem troca de placa.

**Parâmetros de operação.**
- Tensão lógica: **3,3 V** (GPIOs **não são tolerantes a 5 V**).
- Alimentação: 5 V via USB-C ou 3,3 V no pino `3V3`.
- Frequência operada no firmware: 240 MHz (padrão Arduino-ESP32).

**Cuidados.**
- Nunca aplicar 5 V em GPIOs.
- O regulador on-board (AMS1117) fornece 3,3 V com corrente moderada. Em montagens com LoRa transmitindo em potência máxima e HX711, considerar alimentação 3,3 V externa para o LoRa (com GND comum).

---

## 2. Módulo LoRa SX1278 (433 MHz)

**O que é.** Transceptor de rádio LoRa baseado no chip Semtech SX1278, operando na faixa de 433 MHz, com interface SPI.

**Por que foi escolhido.**
- LoRa permite enlace **ponto a ponto sem internet**, com centenas de metros de alcance em zona rural e até **quilômetros em linha de visada**, exatamente o requisito do projeto.
- A faixa de **433 MHz** é permitida no Brasil para aplicações de baixa potência e oferece boa penetração em vegetação.
- Custo baixo e ampla disponibilidade.

**Parâmetros usados no firmware.**

| Parâmetro | Valor |
|---|---|
| Frequência | 433 MHz |
| Sync Word | `0xF3` |
| CRC | Habilitado |
| Interface | SPI |
| Tensão de operação | **3,3 V** |

**Cuidados.**
- **Sempre operar com antena conectada.** Transmitir sem antena pode danificar o estágio de saída de RF.
- **Apenas 3,3 V** na alimentação. 5 V queima o módulo.
- Os dois módulos (TX e RX) precisam usar o **mesmo Sync Word** e **mesma frequência** para se comunicarem.

---

## 3. Sensor AHT10

**O que é.** Sensor digital de temperatura e umidade relativa em encapsulamento único, com interface I²C.

**Por que foi escolhido.**
- Mede as **duas grandezas ambientais mais relevantes para apicultura** (controle de enxameação, perda d'água do mel, estresse térmico) em um só componente.
- Precisão adequada para a aplicação.
- I²C — usa apenas dois fios (SDA, SCL) e compartilha o barramento.
- Custo baixo.

**Parâmetros.**

| Parâmetro | Valor |
|---|---|
| Faixa de temperatura | -40 a 85 °C |
| Precisão de temperatura | ±0,3 °C |
| Faixa de umidade | 0 a 100 % UR |
| Precisão de umidade | ±2 % UR |
| Tensão de operação | 2,0 a 5,5 V (usar 3,3 V no projeto) |
| Endereço I²C | 0x38 |

**Cuidados.**
- Alimentar em **3,3 V** no projeto, mesmo que o sensor suporte 5 V — para manter compatibilidade lógica com o ESP32.
- Posicionar **dentro da colmeia**, longe da saída direta de propólis e do contato com abelhas, mas em local representativo da temperatura interna.

---

## 4. Célula de Carga 50 kg

**O que é.** Sensor de força baseado em strain gauge, em formato de barra, dimensionado para até 50 kg de carga.

**Por que foi escolhido.**
- A faixa de **50 kg** cobre confortavelmente a massa de uma colmeia produtiva (15–40 kg em produção plena).
- Formato em barra facilita instalação sob a base da colmeia.
- Componente clássico, com farta documentação.

**Parâmetros.**

| Parâmetro | Valor típico |
|---|---|
| Capacidade nominal | 50 kg |
| Tensão de alimentação | ~5 V (fornecidos pelo HX711) |
| Sensibilidade | ~1 mV/V |
| Cabos | 4 fios (vermelho, preto, verde, branco) |

**Cuidados.**
- Instalar de modo que a carga incida **perpendicularmente** ao eixo de medição da célula.
- Proteger contra umidade direta e impactos.
- Tarar (zerar) a balança com a colmeia vazia, no momento da instalação.

---

## 5. Módulo HX711

**O que é.** Conversor analógico-digital de 24 bits, projetado especificamente para células de carga com strain gauges. Faz amplificação e digitalização do sinal milivoltímetrico da célula.

**Por que foi escolhido.**
- É o **padrão de fato** para integrar células de carga com microcontroladores.
- 24 bits de resolução são folgados para a precisão necessária.
- Biblioteca Arduino madura.

**Parâmetros.**

| Parâmetro | Valor |
|---|---|
| Resolução | 24 bits |
| Taxa de amostragem | 10 ou 80 SPS (configurável por jumper físico) |
| Tensão de operação | 2,7 a 5,5 V (usar 3,3 V no projeto) |
| Interface | 2 fios (DT, SCK) — protocolo proprietário, não SPI |

**Cuidados.**
- **Calibrar** com pesos conhecidos antes do uso real (procedimento em [`testes.md`](testes.md)).
- Aterrar bem (GND comum com o ESP32) — sem GND comum, a leitura vira ruído.
- A constante `fatorCalibracao` no firmware **é específica de cada célula**. Não copie um valor pronto da internet.

---

## 6. Display OLED SSD1306 0,96"

**O que é.** Display OLED monocromático de 128×64 pixels, com controlador SSD1306 e interface I²C.

**Por que foi escolhido.**
- **Boa legibilidade** em ambiente externo coberto (alto contraste, sem necessidade de retroiluminação).
- Consumo muito baixo.
- Interface I²C simples — compartilha o barramento com outros sensores.
- Suficiente para exibir as 5 linhas de informação da estação central.

**Parâmetros.**

| Parâmetro | Valor |
|---|---|
| Resolução | 128 × 64 pixels |
| Diagonal | 0,96" |
| Tensão de operação | 3,3 V ou 5 V (usar 3,3 V no projeto) |
| Endereço I²C | 0x3C |

**Cuidados.**
- Evitar deixar o mesmo conteúdo estático por longos períodos — OLED sofre *burn-in* com o tempo (não crítico no projeto, pois as leituras mudam continuamente).
- Conferir orientação dos pinos antes de conectar (existe variação entre fabricantes).

---

## 7. Protoboard, Jumpers, Cabos

**Protoboard 400 pontos.** Suficiente para o MVP. Para versão de campo, recomenda-se transferir o circuito para placa universal ou PCB dedicada.

**Jumpers.** Combinação de macho-macho (para protoboard) e macho-fêmea (para módulos com header fêmea, como AHT10 e LoRa). Em montagem definitiva, substituir por cabos soldados.

**Cabos USB-C.** Um para cada ESP32. Para uso de campo, considerar powerbank no transmissor.

---

## 8. Considerações de Custo e Acessibilidade

| Componente | Faixa de preço (BRL) |
|---|---|
| ESP32 WROOM DevKit | R$ 40 – 70 |
| Módulo LoRa SX1278 433 MHz | R$ 50 – 80 |
| Antena helicoidal 433 MHz | R$ 5 – 15 |
| Sensor AHT10 | R$ 15 – 30 |
| Célula de carga 50 kg | R$ 30 – 50 |
| HX711 | R$ 10 – 20 |
| Display OLED SSD1306 0,96" | R$ 25 – 45 |
| Protoboard + jumpers + cabos | R$ 30 – 60 |

**Total estimado do kit completo:** R$ 350 a R$ 500 (preços de referência, mercado brasileiro, 2026).

A escolha dos componentes priorizou **acessibilidade no varejo nacional**, evitando peças exclusivas de importação que travariam a replicabilidade do projeto em outras escolas técnicas e cooperativas.
