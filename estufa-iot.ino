/**
 * Estufa IoT com ESP8266 NodeMCU
 * 
 * @author Thiago Paes <mrprompt@gmail.com>
 */
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DHT.h>

// constantes
#define SSID_REDE                   "mimimi"  // nome da rede wifi
#define SENHA_REDE                  "phplandia"  // senha da rede wifi
#define INTERVALO_ENVIO_THINGSPEAK  30000  // intervalo entre envios de dados ao ThingSpeak (em ms)
#define INTERVALO_LEITURA_SENSORES  120000  // intervalo entre leitura do sensor (em ms)
#define DHT_TYPE                    DHT11 // tipo de sensor DHT utilizado
#define DHT_DATA_PIN_2              D7    // ligação ao pino de dados do sensor
#define DHT_DATA_PIN_1              D8    // ligação ao pino de dados do sensor
#define RELE_PIN                    D1

// variáveis globais
char EnderecoAPIThingSpeak[]  = "api.thingspeak.com"; // endereço do thingspeak
String ChaveEscritaThingSpeak = "G67X1P1QQHT4D7P8"; //chave de escrita do canal
long ultimaConexao;
long ultimaLeitura;
bool luzAcesa;
int UmidadeInternaTruncada;
int TemperaturaInternaTruncada;
int UmidadeExternaTruncada;
int TemperaturaExternaTruncada;
int16_t utc = -3; // UTC -3:00 Brazil
uint32_t currentMillis = 0;
uint32_t previousMillis = 0;
String horaAtual;

// iniciando módulos
WiFiClient client;
WiFiUDP ntpUDP;

DHT sensor_1(DHT_DATA_PIN_1, DHT_TYPE);
DHT sensor_2(DHT_DATA_PIN_2, DHT_TYPE);

NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);

void connectWifi(void)
{
  Serial.print("- Conectando-se à rede WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_REDE, SENHA_REDE);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    Serial.print(".");
  }

  ArduinoOTA.onStart([]() {
    Serial.println("- Inicio...");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("- Fim!");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("- Progresso: %u%%r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("- Erro [%u]: ", error);

    if (error == OTA_AUTH_ERROR) Serial.println("Autenticacao Falhou");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha no Inicio");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha na Conexao");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha na Recepcao");
    else if (error == OTA_END_ERROR) Serial.println("Falha no Fim");
  });

  ArduinoOTA.begin();

  Serial.println("");
  Serial.println("- WiFi connectado com sucesso!");
  Serial.print("- IP obtido: ");
  Serial.print(WiFi.localIP());
  Serial.println();

  delay(1000);
}

void leSensores()
{
  if (ultimaLeitura > 0 && (millis() - ultimaLeitura < INTERVALO_LEITURA_SENSORES)) {
    return;
  }
  
  float UmidadeInterna;
  float TemperaturaInterna;
  float UmidadeExterna;
  float TemperaturaExterna;

  Serial.print("- Lendo sensor interno: ");
  
  UmidadeInterna = sensor_1.readHumidity();
  TemperaturaInterna = sensor_1.readTemperature();
  UmidadeInternaTruncada = (int)UmidadeInterna;
  TemperaturaInternaTruncada = (int)TemperaturaInterna;

  if (isnan(UmidadeInterna) || isnan(TemperaturaInterna)) {
    UmidadeInterna = 0;
    TemperaturaInterna = 0;
  }

  Serial.print("Temperatura: ");
  Serial.print(TemperaturaInterna);
  Serial.print("ºC - Umidade: ");
  Serial.print(UmidadeInterna);
  Serial.println("%");

  Serial.print("- Lendo sensor externo: ");
  
  UmidadeExterna = sensor_2.readHumidity();
  TemperaturaExterna = sensor_2.readTemperature();
  UmidadeExternaTruncada = (int)UmidadeExterna;
  TemperaturaExternaTruncada = (int)TemperaturaExterna;

  if (isnan(UmidadeExterna) || isnan(TemperaturaExterna)) {
    UmidadeExterna = 0;
    TemperaturaExterna = 0;
  }

  ultimaLeitura = millis();

  Serial.print("Temperatura: ");
  Serial.print(TemperaturaExterna);
  Serial.print("ºC - Umidade: ");
  Serial.print(UmidadeExterna);
  Serial.println("%");
}

void enviaDados()
{
  char Fields[100];
  
  //verifica se está conectado no WiFi e se é o momento de enviar dados ao ThingSpeak
  if (!client.connected() && (millis() - ultimaConexao > INTERVALO_ENVIO_THINGSPEAK))
  {
    sprintf(
      Fields, 
      "field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d", 
      UmidadeInternaTruncada, 
      TemperaturaInternaTruncada,
      UmidadeExternaTruncada, 
      TemperaturaExternaTruncada,
      0,
      0
    );

    post(Fields);
  }
}

void post(String StringDados)
{
  if (client.connect(EnderecoAPIThingSpeak, 80))
  {
    //faz a requisição HTTP ao ThingSpeak
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + ChaveEscritaThingSpeak + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(StringDados.length());
    client.print("\n\n");
    client.print(StringDados);

    ultimaConexao = millis();

    Serial.println("- Informações enviadas ao ThingSpeak!");
  }

  client.stop();
}

void controlaLampada()
{
  Serial.println("- Controlando luz");
  
  int horaAtual = timeClient.getHours();

  switch (horaAtual) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
      digitalWrite(RELE_PIN, HIGH);

      luzAcesa = true;
      break;

    default:
      digitalWrite(RELE_PIN, LOW);

      luzAcesa = false;
  }

  delay(200);
}

void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println("- Estufa IoT com ESP8266 NodeMCU");

  ultimaConexao = 0;
  ultimaLeitura = 0;
  UmidadeInternaTruncada = 0;
  TemperaturaInternaTruncada = 0;
  luzAcesa = false;

  connectWifi();

  sensor_1.begin();
  sensor_2.begin();

  timeClient.begin();

  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, LOW);
  delay(200);
}

void loop()
{
  ArduinoOTA.handle();

  timeClient.update();

  leSensores();

  controlaLampada();

  enviaDados();

  delay(5000);
}

