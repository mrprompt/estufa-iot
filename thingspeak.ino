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
      (int)luzAcesa,
      (int)ventilacaoLigada
    );

    Serial.print("- Enviando dados: ");
    Serial.println(Fields);

    post(Fields);
  }
}

void recebeDados()
{
  //verifica se está conectado no WiFi e se é o momento de recuperar os dados do ThingSpeak
  if (!client.connected() && (millis() - ultimaConexaoLeitura > INTERVALO_LEITURA_THINGSPEAK))
  {    
    String comando = get();

    if (comando == "LIGAR_VENTILADOR" || comando == "DESLIGAR_VENTILADOR") {
        comandoVentilacao = comando;
    }

    if (comando == "LIGAR_LUZ" || comando == "DESLIGAR_LUZ") {
        comandoLuz = comando;
    }
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

String get()
{
  String talkBackCommand;
  
  if (client.connect(EnderecoAPIThingSpeak, 80))
  {
    //faz a requisição HTTP ao ThingSpeak
    client.print("GET /talkbacks/" + talkBackID + "/commands/execute?api_key=" + talkBackAPIKey + " HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("\n");

    delay(1000);

    while (client.available()) {
      String line = client.readStringUntil('\n');

      //actual content starts after empty line (that has length 1)
      if (line.length() == 1) {
        talkBackCommand = client.readStringUntil('\n');
        
        break;
      }
    }

    ultimaConexaoLeitura = millis();

    if (talkBackCommand.length() > 1) {
      Serial.print("- Comando: ");
      Serial.println(talkBackCommand);
    }

    client.stop();
  } else {
    Serial.println("- Connection failed");
  }

  return talkBackCommand;
}


