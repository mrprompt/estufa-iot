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
#define SSID_REDE                   "mimimi"      // nome da rede wifi
#define SENHA_REDE                  "phplandia"  // senha da rede wifi
#define INTERVALO_ENVIO_THINGSPEAK  30000   // intervalo entre envios de dados ao ThingSpeak (em ms)
#define INTERVALO_LEITURA_SENSORES  120000  // intervalo entre leitura do sensor (em ms)
#define DHT_TYPE                    DHT11   // tipo de sensor DHT utilizado
#define DHT_DATA_PIN_1              D1      // ligação ao pino de dados do sensor
#define DHT_DATA_PIN_2              D2      // ligação ao pino de dados do sensor
#define RELE_PIN                    D3      // ligação ao pino de controle do relé
#define FAN_PIN                     D8      // ligação ao pino de controle do ventiladores
#define MAX_TEMP                    25      // temperatura máxima, para ligar os ventiladores

// variáveis globais
char EnderecoAPIThingSpeak[]  = "api.thingspeak.com"; // endereço do thingspeak
String ChaveEscritaThingSpeak = "G67X1P1QQHT4D7P8"; //chave de escrita do canal
String horaAtual;
long ultimaConexao = 0;
long ultimaLeitura = 0;
bool luzAcesa = false;
bool ventilacaoLigada = false;
int UmidadeInternaTruncada = 0;
int TemperaturaInternaTruncada = 0;
int UmidadeExternaTruncada = 0;
int TemperaturaExternaTruncada = 0;
int16_t utc = -3; // UTC -3:00 Brazil
uint32_t currentMillis = 0;
uint32_t previousMillis = 0;

// iniciando módulos
WiFiClient client;
WiFiUDP ntpUDP;

DHT sensor_1(DHT_DATA_PIN_1, DHT_TYPE);
DHT sensor_2(DHT_DATA_PIN_2, DHT_TYPE);

NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);

void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println("- Estufa IoT com ESP8266 NodeMCU");

  pinMode(RELE_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
    
  digitalWrite(RELE_PIN, HIGH);
  
  delay(200);

  connectWifi();

  sensor_1.begin();
  sensor_2.begin();

  timeClient.begin();
}

void loop()
{
  ArduinoOTA.handle();

  timeClient.update();

  leSensores();

  controlaVentiladores();

  controlaLampada();

  enviaDados();

  delay(5000);
}

