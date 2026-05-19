#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include "HX711.h"

// =======================
// PINOS DO LORA
// =======================
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  26

// =======================
// PINOS DO I2C
// =======================
#define I2C_SDA 21
#define I2C_SCL 22

// =======================
// PINOS DO HX711
// =======================
#define HX711_DT  32
#define HX711_SCK 33

// =======================
// CONFIGURAÇÕES DO PROJETO
// =======================
#define FREQUENCIA_LORA 433E6
#define ID_COLMEIA 1

Adafruit_AHTX0 aht;
HX711 balanca;

int contadorPacotes = 0;

// Esse valor será ajustado depois na calibração.
// Por enquanto, serve para o MVP funcionar.
// Se o peso aparecer negativo, troque para positivo.
float fatorCalibracao = -7050.0;

void iniciarAHT10() {
  Serial.println("Iniciando sensor AHT10...");

  if (!aht.begin()) {
    Serial.println("ERRO: AHT10 nao encontrado.");
    Serial.println("Verifique VCC, GND, SDA e SCL.");
  } else {
    Serial.println("AHT10 iniciado com sucesso.");
  }
}

void iniciarBalanca() {
  Serial.println("Iniciando HX711...");

  balanca.begin(HX711_DT, HX711_SCK);
  balanca.set_scale(fatorCalibracao);

  Serial.println("Fazendo tara da balanca...");
  Serial.println("Nao coloque peso na celula de carga agora.");
  delay(2000);

  balanca.tare();

  Serial.println("HX711 iniciado e zerado.");
}

void iniciarLoRa() {
  Serial.println("Iniciando LoRa...");

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(FREQUENCIA_LORA)) {
    Serial.println("ERRO: LoRa nao iniciou.");
    Serial.println("Verifique os fios do modulo LoRa.");
    while (true);
  }

  LoRa.setSyncWord(0xF3);
  LoRa.enableCrc();

  Serial.println("LoRa iniciado com sucesso.");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==================================");
  Serial.println("SERTAO BEE - MODULO DA COLMEIA");
  Serial.println("==================================");

  Wire.begin(I2C_SDA, I2C_SCL);

  iniciarAHT10();
  iniciarBalanca();
  iniciarLoRa();

  Serial.println("Modulo da colmeia pronto.");
  Serial.println();
}

void loop() {
  contadorPacotes++;

  // Leitura do AHT10
  sensors_event_t umidadeEvento, temperaturaEvento;
  aht.getEvent(&umidadeEvento, &temperaturaEvento);

  float temperatura = temperaturaEvento.temperature;
  float umidade = umidadeEvento.relative_humidity;

  // Leitura do peso
  float peso = 0.0;

  if (balanca.is_ready()) {
    peso = balanca.get_units(5);
  } else {
    Serial.println("Aviso: HX711 nao esta pronto.");
  }

  // Pacote simples para facilitar leitura e explicacao
  // Formato:
  // SBEE,ID,TEMP,UMIDADE,PESO,CONTADOR
  String pacote = "SBEE," +
                  String(ID_COLMEIA) + "," +
                  String(temperatura, 1) + "," +
                  String(umidade, 1) + "," +
                  String(peso, 2) + "," +
                  String(contadorPacotes);

  // Envio via LoRa
  LoRa.beginPacket();
  LoRa.print(pacote);
  LoRa.endPacket();

  Serial.print("Pacote enviado: ");
  Serial.println(pacote);

  delay(3000);
}