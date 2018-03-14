#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DHT.h>

//defines
#define SSID_REDE                   ""  // nome da rede wifi
#define SENHA_REDE                  ""  // senha da rede wifi
#define INTERVALO_ENVIO_THINGSPEAK  30000  // intervalo entre envios de dados ao ThingSpeak (em ms)
#define DHT_DATA_PIN                D8    // ligação ao pino de dados do sensor
#define DHT_TYPE                    DHT11 // tipo de sensor DHT utilizado

//constantes e variáveis globais
char EnderecoAPIThingSpeak[] = "api.thingspeak.com"; // endereço do thingspeak
String ChaveEscritaThingSpeak = ""; //chave de escrita do canal
long lastConnectionTime;
int UmidadeTruncada;
int TemperaturaTruncada;

WiFiClient client;
ESP8266WebServer server(80);

DHT dht(DHT_DATA_PIN, DHT_TYPE);

/*
   Prototypes
*/
void post(String StringDados);
void connectWifi(void);

/**
   Envia post pro ThingSpeak
*/
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

    lastConnectionTime = millis();

    Serial.println("- Informações enviadas ao ThingSpeak!");
  }

  client.stop();
}

void connectWifi(void)
{
  Serial.println("");
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


void handleRoot() {
  char html[2000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  // Build an HTML page to display on the web-server root address
  snprintf ( html, 2000,
    "<!DOCTYPE html>\
    <html>\
    <head>\
      <meta charset='UTF-8' />\
      <meta http-equiv='refresh' content='10'/>\
      <title>Estufa 01</title>\
      <link rel='stylesheet' href='//maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css' integrity='sha384-1q8mTJOASx8j1Au+a5WDVnPi2lkFfwwEAa8hDDdjZlpLegxhjVME1fgjWPGmkzs7' crossorigin='anonymous'/>\
      <link rel='stylesheet' href='//maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap-theme.min.css' integrity='sha384-fLW2N01lMqjakBkx3l/M9EahuwpSfeNvV63J5ezn3uZzapT0u7EYsXMjQV+0En5r' crossorigin='anonymous'/>\
    </head>\
    <body style='padding-top: 1em'>\
      <div class='container'>\
        <div class='alert alert-info'>Esta página será recarregada em 10 segundos</div>\
        <hr class='clearfix'/>\
        <table class='table table-bordered'>\
          <thead>\
            <tr>\
              <th><span class='glyphicon glyphicon-heart'></span> Uptime </th>\
              <th><span class='glyphicon glyphicon-tree-conifer'></span> Temperatura </th>\
              <th><span class='glyphicon glyphicon-tint'></span> Umidade</th>\
            </tr>\
          </thead>\
          <tbody>\
            <tr>\
              <td> %02d:%02d:%02d </td>\
              <td> %d <sup>o</sup>C </td>\
              <td> %d % </td>\
            </tr>\
          </tbody>\
        </table>\
      </div>\
    </body>\
    </html>",
    hr, min % 60, sec % 60,
    TemperaturaTruncada,
    UmidadeTruncada
  );
  
  server.send(200, "text/html", html);
}

void setup()
{
  Serial.begin(9600);

  lastConnectionTime = 0;
  UmidadeTruncada = 0;
  TemperaturaTruncada = 0;

  connectWifi();

  dht.begin();

  Serial.println("- Estufa IoT com ESP8266 NodeMCU");
}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();

  char Fields[40];
  float Umidade;
  float Temperatura;

  Umidade = dht.readHumidity();
  Temperatura = dht.readTemperature();

  server.on("/", handleRoot);
  server.begin();
  
  Serial.println("- HTTP server started");

  // Check if any reads failed and exit early (to try again).
  if (isnan(Umidade) || isnan(Temperatura)) {
    Serial.println("- Erro: falha ao ler o sensor DHT");

    delay(500);

    return;
  }

  Serial.print("- Temperatura: ");
  Serial.print(Temperatura);
  Serial.print("ºC - Umidade: ");
  Serial.print(Umidade);
  Serial.println("%");

  UmidadeTruncada = (int)Umidade;
  TemperaturaTruncada = (int)Temperatura;

  //verifica se está conectado no WiFi e se é o momento de enviar dados ao ThingSpeak
  if (!client.connected() && (millis() - lastConnectionTime > INTERVALO_ENVIO_THINGSPEAK))
  {
    sprintf(Fields, "field1=%d&field2=%d", UmidadeTruncada, TemperaturaTruncada);

    post(Fields);
  }

  delay(5000);
}

