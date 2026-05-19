#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Iniciando receptor...");

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Erro ao iniciar LoRa");
    while (true);
  }

  LoRa.setSyncWord(0xF3);

  Serial.println("Receptor iniciado!");
  Serial.println("Aguardando mensagens...");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String mensagem = "";

    while (LoRa.available()) {
      mensagem += (char)LoRa.read();
    }

    Serial.print("Recebido: ");
    Serial.println(mensagem);

    Serial.print("Sinal RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
}