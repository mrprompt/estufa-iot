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
