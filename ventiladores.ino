void controlaVentiladores()
{
  Serial.println("- Controlando ventiladores");

  if (TemperaturaInternaTruncada >= MAX_TEMP) {
    ligaVentiladores();
  } else {
    desligaVentiladores();
  }
}

void ligaVentiladores()
{
  digitalWrite(FAN_PIN, HIGH);

  ventilacaoLigada = true;
}

void desligaVentiladores()
{
  digitalWrite(FAN_PIN, LOW);

  ventilacaoLigada = false;
}

