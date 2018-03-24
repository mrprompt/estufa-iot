void leSensores()
{
  if (ultimaLeitura > 0 && (millis() - ultimaLeitura < INTERVALO_LEITURA_SENSORES)) {
    return;
  }
  
  sensorInterno();
  sensorExterno();
}

void sensorInterno()
{  
  float UmidadeInterna;
  float TemperaturaInterna;
 
  Serial.print("- Lendo sensor interno: ");
  
  UmidadeInterna = sensor_1.readHumidity();
  TemperaturaInterna = sensor_1.readTemperature();
  UmidadeInternaTruncada = (int)UmidadeInterna - 13;
  TemperaturaInternaTruncada = (int)TemperaturaInterna - 6;

  if (isnan(UmidadeInterna) || isnan(TemperaturaInterna)) {
    UmidadeInterna = 0;
    TemperaturaInterna = 0;
  }

  Serial.print("Temperatura: ");
  Serial.print(TemperaturaInterna - 6);
  Serial.print("ºC - Umidade: ");
  Serial.print(UmidadeInterna - 13);
  Serial.println("%");
}

void sensorExterno()
{
  float UmidadeExterna;
  float TemperaturaExterna;

  Serial.print("- Lendo sensor externo: ");
  
  UmidadeExterna = sensor_2.readHumidity();
  TemperaturaExterna = sensor_2.readTemperature();
  UmidadeExternaTruncada = (int)UmidadeExterna - 8;
  TemperaturaExternaTruncada = (int)TemperaturaExterna -5;

  if (isnan(UmidadeExterna) || isnan(TemperaturaExterna)) {
    UmidadeExterna = 0;
    TemperaturaExterna = 0;
  }

  ultimaLeitura = millis();

  Serial.print("Temperatura: ");
  Serial.print(TemperaturaExterna - 5);
  Serial.print("ºC - Umidade: ");
  Serial.print(UmidadeExterna - 8);
  Serial.println("%");
}
