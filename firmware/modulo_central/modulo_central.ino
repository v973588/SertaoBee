#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
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
// CONFIGURAÇÕES DO LORA
// =======================
#define FREQUENCIA_LORA 433E6
 
// =======================
// CONFIGURAÇÕES DO WIFI LOCAL
// =======================
const char* nomeRede = "SertaoBee";
const char* senhaRede = "12345678";
 
WebServer server(80);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
// =======================
// VARIÁVEIS DOS DADOS RECEBIDOS
// =======================
String idColmeia = "--";
String temperatura = "--";
String umidade = "--";
String peso = "--";
String contador = "--";
String ultimoPacote = "Nenhum pacote recebido ainda";
 
int rssi = 0;
unsigned long ultimoRecebimento = 0;
 
// =======================
// FUNÇÃO PARA PEGAR CAMPOS DO PACOTE
// Formato esperado:
// SBEE,1,26.5,63.0,0.00,10
// =======================
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
 
// =======================
// MOSTRAR MENSAGEM SIMPLES NO OLED
// =======================
void mostrarMensagem(String linha1, String linha2, String linha3 = "", String linha4 = "") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
 
  display.setCursor(0, 0);
  display.println(linha1);
 
  display.setCursor(0, 16);
  display.println(linha2);
 
  display.setCursor(0, 32);
  display.println(linha3);
 
  display.setCursor(0, 48);
  display.println(linha4);
 
  display.display();
}
 
// =======================
// MOSTRAR DADOS NO OLED
// =======================
void mostrarDadosNaTela() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
 
  display.setCursor(0, 0);
  display.print("SERTAO BEE ID:");
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
  display.print(" P:");
  display.print(contador);
 
  display.display();
}
 
// =======================
// PÁGINA HTML PRINCIPAL
// =======================
String gerarPaginaHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Sertão Bee</title>
  <style>
    * {
      box-sizing: border-box;
      font-family: Arial, sans-serif;
    }
 
    body {
      margin: 0;
      background: #f6f1df;
      color: #222;
    }
 
    header {
      background: #f7b733;
      padding: 22px 18px;
      text-align: center;
      color: #2b2100;
      border-bottom: 4px solid #2f7d32;
    }
 
    header h1 {
      margin: 0;
      font-size: 28px;
    }
 
    header p {
      margin: 8px 0 0;
      font-size: 14px;
    }
 
    .container {
      max-width: 900px;
      margin: 20px auto;
      padding: 0 14px;
    }
 
    .status {
      background: white;
      border-radius: 14px;
      padding: 16px;
      margin-bottom: 16px;
      box-shadow: 0 3px 10px rgba(0,0,0,0.12);
      border-left: 6px solid #2f7d32;
    }
 
    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
      gap: 14px;
    }
 
    .card {
      background: white;
      border-radius: 14px;
      padding: 18px;
      text-align: center;
      box-shadow: 0 3px 10px rgba(0,0,0,0.12);
    }
 
    .label {
      font-size: 14px;
      color: #555;
      margin-bottom: 8px;
    }
 
    .value {
      font-size: 30px;
      font-weight: bold;
      color: #2f7d32;
    }
 
    .small {
      font-size: 13px;
      color: #666;
      margin-top: 8px;
      word-break: break-word;
    }
 
    footer {
      text-align: center;
      color: #666;
      font-size: 12px;
      margin: 24px 0;
    }
  </style>
</head>
<body>
  <header>
    <h1>Sertão Bee</h1>
    <p>Monitoramento offline de colmeias com ESP32 e LoRa</p>
  </header>
 
  <div class="container">
    <div class="status">
      <strong>Status:</strong> <span id="status">Aguardando dados...</span><br>
      <span class="small">Rede local criada pelo ESP32. Não precisa de internet.</span>
    </div>
 
    <div class="grid">
      <div class="card">
        <div class="label">Colmeia</div>
        <div class="value" id="idColmeia">--</div>
      </div>
 
      <div class="card">
        <div class="label">Temperatura</div>
        <div class="value"><span id="temperatura">--</span> °C</div>
      </div>
 
      <div class="card">
        <div class="label">Umidade</div>
        <div class="value"><span id="umidade">--</span> %</div>
      </div>
 
      <div class="card">
        <div class="label">Peso</div>
        <div class="value"><span id="peso">--</span> kg</div>
      </div>
 
      <div class="card">
        <div class="label">Sinal LoRa</div>
        <div class="value"><span id="rssi">--</span></div>
        <div class="small">RSSI em dBm</div>
      </div>
 
      <div class="card">
        <div class="label">Pacote</div>
        <div class="value" id="contador">--</div>
      </div>
    </div>
 
    <div class="status" style="margin-top:16px;">
      <strong>Último pacote recebido:</strong>
      <div class="small" id="ultimoPacote">Nenhum pacote recebido ainda</div>
    </div>
  </div>
 
  <footer>
    Sertão Bee | MVP Hackathon | ESP32 + LoRa + OLED
  </footer>
 
  <script>
    async function atualizarDados() {
      try {
        const resposta = await fetch('/dados');
        const dados = await resposta.json();
 
        document.getElementById('idColmeia').innerText = dados.id;
        document.getElementById('temperatura').innerText = dados.temperatura;
        document.getElementById('umidade').innerText = dados.umidade;
        document.getElementById('peso').innerText = dados.peso;
        document.getElementById('rssi').innerText = dados.rssi;
        document.getElementById('contador').innerText = dados.contador;
        document.getElementById('ultimoPacote').innerText = dados.ultimoPacote;
 
        if (dados.recebeuDados) {
          document.getElementById('status').innerText = "Recebendo dados da colmeia";
        } else {
          document.getElementById('status').innerText = "Aguardando primeiro pacote LoRa";
        }
      } catch (erro) {
        document.getElementById('status').innerText = "Erro ao atualizar dados";
      }
    }
 
    setInterval(atualizarDados, 2000);
    atualizarDados();
  </script>
</body>
</html>
)rawliteral";
 
  return html;
}
 
// =======================
// ENDPOINT DA PÁGINA
// =======================
void handlePaginaPrincipal() {
  server.send(200, "text/html", gerarPaginaHTML());
}
 
// =======================
// ENDPOINT JSON DOS DADOS
// =======================
void handleDados() {
  bool recebeuDados = ultimoRecebimento > 0;
 
  String json = "{";
  json += "\"id\":\"" + idColmeia + "\",";
  json += "\"temperatura\":\"" + temperatura + "\",";
  json += "\"umidade\":\"" + umidade + "\",";
  json += "\"peso\":\"" + peso + "\",";
  json += "\"contador\":\"" + contador + "\",";
  json += "\"rssi\":\"" + String(rssi) + "\",";
  json += "\"ultimoPacote\":\"" + ultimoPacote + "\",";
  json += "\"recebeuDados\":" + String(recebeuDados ? "true" : "false");
  json += "}";
 
  server.send(200, "application/json", json);
}
 
// =======================
// INICIAR WIFI LOCAL
// =======================
void iniciarWiFiLocal() {
  Serial.println("Criando rede Wi-Fi local...");
 
  WiFi.mode(WIFI_AP);
  WiFi.softAP(nomeRede, senhaRede);
 
  IPAddress ip = WiFi.softAPIP();
 
  Serial.print("Rede criada: ");
  Serial.println(nomeRede);
 
  Serial.print("Senha: ");
  Serial.println(senhaRede);
 
  Serial.print("Acesse: http://");
  Serial.println(ip);
 
  server.on("/", handlePaginaPrincipal);
  server.on("/dados", handleDados);
  server.begin();
 
  Serial.println("Servidor web iniciado.");
 
  mostrarMensagem("SERTAO BEE", "WiFi: SertaoBee", "Acesse:", "192.168.4.1");
}
 
// =======================
// INICIAR OLED
// =======================
void iniciarOLED() {
  Serial.println("Iniciando display OLED...");
 
  Wire.begin(I2C_SDA, I2C_SCL);
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERRO: OLED nao encontrado.");
    while (true);
  }
 
  Serial.println("OLED iniciado com sucesso.");
  mostrarMensagem("SERTAO BEE", "Iniciando...", "", "");
}
 
// =======================
// INICIAR LORA
// =======================
void iniciarLoRa() {
  Serial.println("Iniciando LoRa...");
 
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
 
  if (!LoRa.begin(FREQUENCIA_LORA)) {
    Serial.println("ERRO: LoRa nao iniciou.");
    Serial.println("Verifique os fios do modulo LoRa.");
 
    mostrarMensagem("ERRO LORA", "Verifique fios", "", "");
 
    while (true);
  }
 
  LoRa.setSyncWord(0xF3);
  LoRa.enableCrc();
 
  Serial.println("LoRa iniciado com sucesso.");
}
 
// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  delay(1000);
 
  Serial.println();
  Serial.println("==================================");
  Serial.println("SERTAO BEE - ESTACAO CENTRAL WEB");
  Serial.println("==================================");
 
  iniciarOLED();
  iniciarWiFiLocal();
  iniciarLoRa();
 
  Serial.println("Estacao central pronta.");
  Serial.println("Aguardando pacotes LoRa...");
  Serial.println();
}
 
// =======================
// LOOP
// =======================
void loop() {
  server.handleClient();
 
  int tamanhoPacote = LoRa.parsePacket();
 
  if (tamanhoPacote) {
    String pacote = "";
 
    while (LoRa.available()) {
      pacote += (char)LoRa.read();
    }
 
    rssi = LoRa.packetRssi();
    ultimoPacote = pacote;
    ultimoRecebimento = millis();
 
    Serial.println();
    Serial.print("Pacote recebido: ");
    Serial.println(pacote);
 
    Serial.print("RSSI: ");
    Serial.println(rssi);
 
    String tipo = pegarCampo(pacote, 0);
 
    if (tipo == "SBEE") {
      idColmeia = pegarCampo(pacote, 1);
      temperatura = pegarCampo(pacote, 2);
      umidade = pegarCampo(pacote, 3);
      peso = pegarCampo(pacote, 4);
      contador = pegarCampo(pacote, 5);
 
      mostrarDadosNaTela();
 
      Serial.println("Dados atualizados no OLED e na pagina web.");
    } else {
      Serial.println("Pacote ignorado. Formato invalido.");
    }
  }
}