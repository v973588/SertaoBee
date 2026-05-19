#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
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
// CONFIGURAÇÕES DO OLED
// =======================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
 
// =======================
// CONFIGURAÇÕES DO PROJETO
// =======================
#define FREQUENCIA_LORA 433E6
 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
String idColmeia = "--";
String temperatura = "--";
String umidade = "--";
String peso = "--";
String contador = "--";
int rssi = 0;
 
String pegarCampo(String texto, int indice) {
  int campoAtual = 0;
  int inicio = 0;
 
  for (int i = 0; i <= texto.length(); i++) {
    if (texto.charAt(i) == ',' || i == texto.length()) {
      if (campoAtual == indice) {
        return texto.substring(inicio, i);
      }
 
      campoAtual++;
      inicio = i + 1;
    }
  }
 
  return "";
}
 
void mostrarTelaInicial() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
 
  display.setCursor(0, 0);
  display.println("SERTAO BEE");
 
  display.setCursor(0, 16);
  display.println("Estacao Central");
 
  display.setCursor(0, 32);
  display.println("Aguardando dados");
 
  display.display();
}
 
void mostrarDadosNaTela() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
 
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("SERTAO BEE  ID:");
  display.println(idColmeia);
 
  display.setCursor(0, 14);
  display.print("Temp: ");
  display.print(temperatura);
  display.println(" C");
 
  display.setCursor(0, 26);
  display.print("Umid: ");
  display.print(umidade);
  display.println(" %");
 
  display.setCursor(0, 38);
  display.print("Peso: ");
  display.print(peso);
  display.println(" kg");
 
  display.setCursor(0, 52);
  display.print("RSSI:");
  display.print(rssi);
  display.print("  Pct:");
  display.print(contador);
 
  display.display();
}
 
void iniciarOLED() {
  Serial.println("Iniciando display OLED...");
 
  Wire.begin(I2C_SDA, I2C_SCL);
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERRO: OLED nao encontrado no endereco 0x3C.");
    Serial.println("Verifique VCC, GND, SDA e SCL.");
 
    while (true);
  }
 
  Serial.println("OLED iniciado com sucesso.");
  mostrarTelaInicial();
}
 
void iniciarLoRa() {
  Serial.println("Iniciando LoRa...");
 
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
 
  if (!LoRa.begin(FREQUENCIA_LORA)) {
    Serial.println("ERRO: LoRa nao iniciou.");
    Serial.println("Verifique os fios do modulo LoRa.");
 
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("ERRO LORA");
    display.println("Verifique fios");
    display.display();
 
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
  Serial.println("SERTAO BEE - ESTACAO CENTRAL");
  Serial.println("==================================");
 
  iniciarOLED();
  iniciarLoRa();
 
  Serial.println("Estacao central pronta.");
  Serial.println("Aguardando pacotes...");
  Serial.println();
}
 
void loop() {
  int tamanhoPacote = LoRa.parsePacket();
 
  if (tamanhoPacote) {
    String pacote = "";
 
    while (LoRa.available()) {
      pacote += (char)LoRa.read();
    }
 
    rssi = LoRa.packetRssi();
 
    Serial.print("Pacote recebido: ");
    Serial.println(pacote);
 
    Serial.print("RSSI: ");
    Serial.println(rssi);
 
    // Confere se o pacote começa com SBEE
    String identificador = pegarCampo(pacote, 0);
 
    if (identificador == "SBEE") {
      idColmeia = pegarCampo(pacote, 1);
      temperatura = pegarCampo(pacote, 2);
      umidade = pegarCampo(pacote, 3);
      peso = pegarCampo(pacote, 4);
      contador = pegarCampo(pacote, 5);
 
      mostrarDadosNaTela();
 
      Serial.println("Dados exibidos no OLED.");
      Serial.println();
    } else {
      Serial.println("Pacote ignorado. Formato invalido.");
      Serial.println();
    }
  }
}