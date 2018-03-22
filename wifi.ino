void connectWifi(void)
{
  Serial.print("- Conectando-se à rede WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_REDE, SENHA_REDE);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);

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

