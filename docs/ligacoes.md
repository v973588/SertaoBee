# Ligações — Sertão Bee

Este documento descreve, pino a pino, as ligações elétricas dos dois módulos do projeto. Todos os pinos referenciados são **GPIOs do ESP32 WROOM DevKit** e correspondem exatamente aos `#define` do firmware.

> ⚠️ **Antes de energizar:** confira tudo duas vezes, especialmente VCC e GND. Inverter alimentação queima o sensor.

---

## 1. Módulo da Colmeia (Transmissor)

Componentes ligados ao ESP32:

- Sensor **AHT10** (I²C) — temperatura e umidade
- **HX711 + célula de carga 50 kg** — peso
- Módulo **LoRa SX1278 433 MHz** — rádio

### 1.1 Sensor AHT10 (I²C)

| Pino do AHT10 | Pino do ESP32 | Observação |
|---|---|---|
| `VCC` | `3V3` | **Não usar 5 V**, o AHT10 opera em 3,3 V |
| `GND` | `GND` | — |
| `SDA` | `GPIO 21` | Linha de dados I²C |
| `SCL` | `GPIO 22` | Linha de clock I²C |

Endereço I²C padrão do AHT10: **0x38**.

### 1.2 HX711 + célula de carga

#### HX711 → ESP32

| Pino do HX711 | Pino do ESP32 |
|---|---|
| `VCC` | `3V3` *(ou 5V — ver nota)* |
| `GND` | `GND` |
| `DT` (Data) | `GPIO 32` |
| `SCK` (Clock) | `GPIO 33` |

> **Nota sobre alimentação:** o HX711 funciona em 3,3 V e em 5 V. Recomenda-se **3,3 V** quando alimentado pelo regulador do ESP32, para evitar problemas de nível lógico nos pinos DT e SCK. Se for alimentado por 5 V externos, garanta que os sinais lógicos sejam compatíveis com o ESP32 (que **não é tolerante a 5 V** nos GPIOs).

#### Célula de carga → HX711

A célula de carga tem **quatro fios coloridos**. O padrão mais comum é:

| Fio da célula | Pino do HX711 | Função |
|---|---|---|
| Vermelho | `E+` | Excitação positiva |
| Preto | `E-` | Excitação negativa |
| Branco | `A-` | Sinal negativo |
| Verde | `A+` | Sinal positivo |

> Se o peso aparecer **negativo** após a calibração, basta inverter `A+` ↔ `A-` ou inverter o sinal do `fatorCalibracao` no firmware (de `-7050.0` para `+7050.0`, por exemplo).

### 1.3 Módulo LoRa SX1278 (SPI)

| Pino do LoRa | Pino do ESP32 | Função |
|---|---|---|
| `VCC` / `3.3V` | `3V3` | **Apenas 3,3 V.** Alimentar com 5 V queima o módulo. |
| `GND` | `GND` | — |
| `MISO` | `GPIO 19` | SPI MISO |
| `MOSI` | `GPIO 23` | SPI MOSI |
| `SCK` | `GPIO 18` | SPI Clock |
| `NSS` / `CS` | `GPIO 5` | Chip Select |
| `RST` | `GPIO 14` | Reset |
| `DIO0` | `GPIO 26` | Interrupção (fim de transmissão / recepção) |

**Antena.** O módulo LoRa **precisa** de antena para 433 MHz. Operar sem antena pode danificar o estágio de saída de RF. Antena helicoidal soldada no pad `ANT` resolve.

### 1.4 Resumo dos pinos do ESP32 — Módulo da Colmeia

| GPIO | Função |
|---|---|
| 5 | LoRa CS (NSS) |
| 14 | LoRa RST |
| 18 | LoRa SCK |
| 19 | LoRa MISO |
| 21 | I²C SDA (AHT10) |
| 22 | I²C SCL (AHT10) |
| 23 | LoRa MOSI |
| 26 | LoRa DIO0 |
| 32 | HX711 DT |
| 33 | HX711 SCK |
| 3V3 | VCC dos três módulos |
| GND | GND comum |

---

## 2. Estação Central (Receptor)

Componentes ligados ao ESP32:

- Módulo **LoRa SX1278 433 MHz** — rádio
- Display **OLED SSD1306 0,96"** (I²C) — visualização

### 2.1 Módulo LoRa SX1278 (SPI)

A pinagem é **idêntica** à do transmissor (e isso é proposital — facilita manutenção e reposição):

| Pino do LoRa | Pino do ESP32 | Função |
|---|---|---|
| `VCC` / `3.3V` | `3V3` | **Apenas 3,3 V** |
| `GND` | `GND` | — |
| `MISO` | `GPIO 19` | SPI MISO |
| `MOSI` | `GPIO 23` | SPI MOSI |
| `SCK` | `GPIO 18` | SPI Clock |
| `NSS` / `CS` | `GPIO 5` | Chip Select |
| `RST` | `GPIO 14` | Reset |
| `DIO0` | `GPIO 26` | Interrupção |

### 2.2 Display OLED SSD1306 (I²C)

| Pino do OLED | Pino do ESP32 | Observação |
|---|---|---|
| `VCC` | `3V3` | O módulo aceita 3,3 V ou 5 V; com ESP32, use 3,3 V |
| `GND` | `GND` | — |
| `SDA` | `GPIO 21` | Linha de dados I²C |
| `SCL` | `GPIO 22` | Linha de clock I²C |

Endereço I²C padrão do OLED 0,96": **0x3C**.

### 2.3 Resumo dos pinos do ESP32 — Estação Central

| GPIO | Função |
|---|---|
| 5 | LoRa CS (NSS) |
| 14 | LoRa RST |
| 18 | LoRa SCK |
| 19 | LoRa MISO |
| 21 | I²C SDA (OLED) |
| 22 | I²C SCL (OLED) |
| 23 | LoRa MOSI |
| 26 | LoRa DIO0 |
| 3V3 | VCC dos dois módulos |
| GND | GND comum |

---

## 3. Diagrama esquemático (texto)

### Transmissor

```
                              ESP32 WROOM
                       ┌────────────────────────┐
        AHT10  SDA ────┤ 21                  5  ├──── CS    LoRa
        AHT10  SCL ────┤ 22                 14  ├──── RST   LoRa
                       │                        │
        HX711  DT  ────┤ 32                 18  ├──── SCK   LoRa
        HX711  SCK ────┤ 33                 19  ├──── MISO  LoRa
                       │                    23  ├──── MOSI  LoRa
                       │                    26  ├──── DIO0  LoRa
                       │                        │
                       │   3V3 ─── VCC dos módulos
                       │   GND ─── GND comum
                       └────────────────────────┘
```

### Receptor

```
                              ESP32 WROOM
                       ┌────────────────────────┐
        OLED   SDA ────┤ 21                  5  ├──── CS    LoRa
        OLED   SCL ────┤ 22                 14  ├──── RST   LoRa
                       │                    18  ├──── SCK   LoRa
                       │                    19  ├──── MISO  LoRa
                       │                    23  ├──── MOSI  LoRa
                       │                    26  ├──── DIO0  LoRa
                       │                        │
                       │   3V3 ─── VCC dos módulos
                       │   GND ─── GND comum
                       └────────────────────────┘
```

---

## 4. Alimentação

- **MVP / bancada:** alimentação por cabo **USB-C** diretamente nos dois ESP32.
- **Demonstração de campo:** powerbank USB no módulo da colmeia, fonte USB de parede na estação central.
- **Operação prolongada (Fase 3):** painel solar + bateria 18650 com módulo TP4056 + boost converter, ainda não incluído no MVP.

### Cuidados

- O barramento `3V3` do ESP32 não é uma fonte de alta corrente. Conectar AHT10 + HX711 + LoRa funciona, mas pelo limite do regulador on-board.
- Se houver instabilidade (reset durante transmissão LoRa, leitura ruim do HX711), alimentar o LoRa por uma fonte 3,3 V externa dedicada, **compartilhando o GND** com o ESP32.

---

## 5. Verificações antes de ligar

Lista mínima de conferência antes do primeiro power-on:

- [ ] Nenhum fio de alimentação trocado (VCC/GND)
- [ ] Nenhum módulo de 3,3 V conectado em 5 V
- [ ] Antena soldada no módulo LoRa
- [ ] SDA e SCL não cruzados entre módulos I²C
- [ ] GND comum entre todos os módulos
- [ ] Célula de carga com os quatro fios no HX711, sem curto-circuito

Se algo no Serial Monitor exibir `ERRO: ...`, verifique a tabela do componente correspondente neste documento.
