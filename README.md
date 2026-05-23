# 🐝 Sertão Bee

**Sistema Offline de Monitoramento de Colmeias com ESP32 e LoRa**

Apoio à apicultura familiar do semiárido piauiense.

> **CETI Malaquias Ribeiro Damasceno** · São Lourenço do Piauí · 13ª Gerência Regional
> Hackathon 2026 — Programa *Do Piauí para o Mundo*
> Eixo: Agricultura, Agroindústria e Bioeconomia

---

## 📌 Sumário

- [Visão Geral](#visão-geral)
- [O Problema](#o-problema)
- [A Solução](#a-solução)
- [Arquitetura](#arquitetura)
- [Componentes](#componentes)
- [Estrutura do Repositório](#estrutura-do-repositório)
- [Como Reproduzir](#como-reproduzir)
- [Documentação Detalhada](#documentação-detalhada)
- [Equipe](#equipe)
- [Licença](#licença)

---

## Visão Geral

O **Sertão Bee** é uma solução IoT de baixo custo, **totalmente offline**, que monitora **temperatura, umidade e peso** de colmeias e envia esses dados por **rádio LoRa** (433 MHz) para uma **estação central** instalada na sede da propriedade. Lá, o apicultor consulta as informações de duas formas: num **display OLED** (sem precisar de celular) e num **painel web** aberto no navegador do celular, servido por uma **rede Wi-Fi local criada pelo próprio ESP32** — sem internet, sem roteador externo.

> **Importante:** o painel web usa um Wi-Fi *local* gerado pela própria estação central (modo Access Point). Não há internet envolvida em momento algum: o enlace colmeia→estação é por rádio LoRa, e o acesso ao painel é por uma rede privada do dispositivo. O sistema continua 100% offline.

A proposta nasce de um problema real: no semiárido piauiense, a maioria dos apiários fica em zonas **sem sinal de internet**, e o pequeno apicultor familiar toma decisões de manejo apenas por observação direta, em visitas espaçadas. O resultado é perda de produção, risco de enxameação e visitas desnecessárias que estressam as colônias.

O Sertão Bee entrega dados contínuos de manejo **sem depender de internet, sem nuvem, sem assinatura e sem operadora**. O único Wi-Fi usado é uma rede local do próprio ESP32, para o painel no celular — nunca para acessar a internet.

---

## O Problema

O Piauí é o **2º maior produtor de mel do Brasil**, com 12,8% da produção nacional, concentrada na agricultura familiar. Apesar disso, o pequeno apicultor não dispõe de ferramentas tecnológicas acessíveis para acompanhar suas colmeias:

- **Sem dados contínuos.** Temperatura, umidade e peso são observados apenas em visitas presenciais.
- **Sem internet rural.** A maioria dos apiários fica em áreas com sinal instável ou ausente — soluções de nuvem **não funcionam**.
- **Perdas evitáveis.** Sem informação contínua, o apicultor perde o momento ideal de colheita e arrisca enxameação ou enfraquecimento das colônias.

---

## A Solução

Dois módulos que se comunicam por rádio LoRa, **sem internet entre eles**:

### 🟡 Módulo da Colmeia (transmissor)
Instalado na caixa apícola. Lê os sensores e transmite os dados.
- **ESP32 WROOM** — controle e processamento
- **Sensor AHT10** — temperatura e umidade (I²C)
- **HX711 + célula de carga 50 kg** — peso da colmeia
- **LoRa SX1278 (433 MHz)** — transmissão sem fio

### 🟢 Estação Central (receptor)
Instalada na sede da propriedade. Recebe os pacotes e mostra ao apicultor.
- **ESP32 WROOM** — controle, processamento e servidor web local
- **LoRa SX1278 (433 MHz)** — recepção
- **Display OLED SSD1306 0,96"** — leitura local em tempo quase real, sem precisar de celular
- **Painel web local** — rede Wi-Fi `SertaoBee` criada pelo próprio ESP32; o apicultor abre `http://192.168.4.1` no navegador do celular e vê os dados em cards que atualizam sozinhos a cada 2 segundos

> Para diagramas, ligações pino a pino e detalhes de cada componente, veja [`docs/arquitetura.md`](docs/arquitetura.md), [`docs/ligacoes.md`](docs/ligacoes.md) e [`docs/componentes.md`](docs/componentes.md).

---

## Arquitetura

![Arquitetura geral do Sertão Bee: módulo da colmeia (AHT10, HX711, célula de carga, ESP32 + LoRa TX) conectado por rádio LoRa 433 MHz à estação central (ESP32 + LoRa RX), que exibe os dados no display OLED e num painel web acessível pelo celular via Wi-Fi local, sem internet](imagens/arquitetura-geral.png)

Formato do pacote enviado:

```
SBEE,<id_colmeia>,<temperatura_°C>,<umidade_%>,<peso_kg>,<contador>
```

Exemplo: `SBEE,1,32.4,58.2,18.75,142`

---

## Componentes

| Item | Quantidade | Função |
|---|---|---|
| ESP32 WROOM DevKit (USB-C) | 2 | Microcontrolador dos dois módulos |
| Módulo LoRa SX1278 433 MHz | 2 | Rádio para enlace offline |
| Sensor AHT10 (I²C) | 1 | Temperatura e umidade na colmeia |
| Célula de carga 50 kg + HX711 | 1 | Pesagem da colmeia |
| Display OLED SSD1306 0,96" (I²C) | 1 | Visualização na estação central |
| Protoboard 400 pontos | 2 | Montagem do MVP |
| Jumpers macho-macho / macho-fêmea | Diversos | Ligações |
| Antena helicoidal 433 MHz | 2 | Necessária para operação do LoRa |

> Detalhamento, datasheets e considerações de uso: [`docs/componentes.md`](docs/componentes.md).

---

## Estrutura do Repositório

```
sertao-bee/
├── README.md
├── firmware/
│   ├── modulo_colmeia/
│   │   └── modulo_colmeia.ino       # Firmware do transmissor
│   └── estacao_central/
│       └── estacao_central.ino      # Firmware do receptor
├── docs/
│   ├── arquitetura.md               # Visão técnica e fluxo de dados
│   ├── ligacoes.md                  # Pinagem e ligações pino a pino
│   ├── componentes.md               # Detalhamento dos componentes
│   └── testes.md                    # Plano e roteiro de testes
├── imagens/
│   ├── montagem-transmissor.jpg
│   ├── montagem-receptor.jpg
│   └── maquete.jpg
└── slides/
    └── apresentacao-sertao-bee.pdf
```

---

## Como Reproduzir

### Pré-requisitos
- **Arduino IDE** 1.8.19+ ou **Arduino IDE 2.x**
- Suporte à placa **ESP32** instalado no Arduino IDE
  (URL adicional: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`)
- Bibliotecas necessárias (instalar pelo Library Manager):
  - `LoRa` por Sandeep Mistry
  - `Adafruit GFX Library`
  - `Adafruit SSD1306`
  - `Adafruit AHTX0`
  - `HX711` por Bogdan Necula

### Compilação e gravação
1. Monte os circuitos conforme [`docs/ligacoes.md`](docs/ligacoes.md).
2. Abra `firmware/modulo_colmeia/modulo_colmeia.ino` na Arduino IDE, selecione a placa **ESP32 Dev Module** e grave no ESP32 do transmissor.
3. Abra `firmware/estacao_central/estacao_central.ino`, selecione a mesma placa e grave no ESP32 da estação central.
4. Abra o **Serial Monitor** (115200 baud) nos dois módulos para acompanhar o funcionamento.
5. O OLED da estação central deve começar a exibir os dados após o primeiro pacote ser recebido (~3 segundos).

> As bibliotecas `WiFi` e `WebServer`, usadas no painel web, já vêm incluídas no core do ESP32 — não é preciso instalá-las separadamente.

### Acessar o painel web (estação central)
1. No celular, abra as redes Wi-Fi e conecte-se à rede **`SertaoBee`** (senha **`12345678`**), criada pela própria estação central.
2. No navegador, acesse **`http://192.168.4.1`**.
3. O painel mostra temperatura, umidade, peso, sinal LoRa (RSSI) e contador de pacotes, atualizando sozinho a cada 2 segundos. Tudo funciona **sem internet** — é uma rede local do próprio ESP32.

### Calibração da balança
A constante `fatorCalibracao` no firmware do módulo da colmeia precisa ser ajustada para cada célula de carga. O procedimento (com 5+ pontos documentados) está descrito em [`docs/testes.md`](docs/testes.md).

---

## Documentação Detalhada

| Documento | Conteúdo |
|---|---|
| [`docs/arquitetura.md`](docs/arquitetura.md) | Diagrama em blocos, fluxo de dados, formato do pacote LoRa, decisões de projeto |
| [`docs/ligacoes.md`](docs/ligacoes.md) | Tabelas de pinagem do transmissor e do receptor, observações de alimentação |
| [`docs/componentes.md`](docs/componentes.md) | Cada componente: o que é, por que foi escolhido, parâmetros relevantes |
| [`docs/testes.md`](docs/testes.md) | Roteiro de testes, calibração da balança, métricas de validação |

---

## Equipe

**CETI Malaquias Ribeiro Damasceno — São Lourenço do Piauí**

- Víctor Manoel Xavier Santana
- Victor Hugo Vilanova Gameleira
- Josiel Xavier Santana Paes
- Kamel de Santana Silva
- Pedro Henrique Pereira Damasceno

**Orientador:** Prof. Danilo Gameleira Dias

---

## Licença

Este projeto é distribuído sob licença **MIT**, aberta para fins educacionais, de pesquisa e de extensão. A arquitetura é propositadamente replicável por escolas técnicas, cooperativas e associações de apicultores.

---

*Sertão Bee — Uma ponte entre a tecnologia contemporânea e a realidade da produção familiar no semiárido.*
