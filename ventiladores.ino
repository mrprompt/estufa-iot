void controlaVentiladores()
{
  Serial.println("- Controlando ventiladores");
  
  if (TemperaturaInternaTruncada >= 25) {
    digitalWrite(FAN_PIN, HIGH);
    delay(200);

    ventilacaoLigada = true;
  } else {
    digitalWrite(FAN_PIN, LOW);
    delay(200);

    ventilacaoLigada = false;
  }
}

