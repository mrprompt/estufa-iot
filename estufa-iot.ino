#include <ESP8266WiFi.h>
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

WiFiClient client;
DHT dht(DHT_DATA_PIN, DHT_TYPE);

/*
 * Prototypes
 */
void post(String StringDados);
void connect(void);
float humidity(void);
float temperature(void);

/*
 * Implementações
 */
void post(String StringDados)
{
    if (client.connect(EnderecoAPIThingSpeak, 80))
    {         
        //faz a requisição HTTP ao ThingSpeak
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+ChaveEscritaThingSpeak+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(StringDados.length());
        client.print("\n\n");
        client.print(StringDados);
  
        lastConnectionTime = millis();
        Serial.println("- Informações enviadas ao ThingSpeak!");
     }   
}

void connect(void)
{
    client.stop();

    Serial.println("");
    Serial.println("- Conectando-se à rede WiFi...");
    
    delay(1000);
    
    WiFi.begin(SSID_REDE, SENHA_REDE);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("- WiFi connectado com sucesso!");  
    Serial.print("- IP obtido: ");
    Serial.print(WiFi.localIP());
    Serial.println();

    delay(1000);
}

float humidity(void)
{
  return dht.readHumidity();
}

float temperature(void) {
  return dht.readTemperature();
}

void setup()
{  
    Serial.begin(9600);
    
    lastConnectionTime = 0; 
    
    connect();

    dht.begin();
    
    Serial.println("- Estufa IoT com ESP8266 NodeMCU");
}

void loop()
{
    int Umidade;
    int Temperatura;
    char Fields[40];
    
    //Força desconexão ao ThingSpeak (se ainda estiver desconectado)
    if (client.connected())
    {
        client.stop();
        
        Serial.println("- Desconectado do ThingSpeak");
        Serial.println();
    }

    Umidade = (int)humidity();
    Temperatura = (int)temperature();

    Serial.printf("- Temperatura: %d", Temperatura);
    Serial.println();
    Serial.printf("- Umidade do ar: %d", Umidade);
    Serial.println();
    
    //verifica se está conectado no WiFi e se é o momento de enviar dados ao ThingSpeak
    if (!client.connected() && (millis() - lastConnectionTime > INTERVALO_ENVIO_THINGSPEAK))
    {
        sprintf(Fields,"field1=%d&field2=%d", Umidade, Temperatura);
      
        post(Fields);
    }

     delay(1000);
}

