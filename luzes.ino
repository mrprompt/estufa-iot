void controlaLampada()
{
  Serial.println("- Controlando luz");
  
  int horaAtual = timeClient.getHours();

  if (horaAtual >= 6 && horaAtual <= 20) {
      ligaLuz();
  } else {
      desligaLuz();
  }
}

void ligaLuz()
{
  digitalWrite(RELE_PIN, LOW);

  luzAcesa = true;  
}

void desligaLuz()
{
  digitalWrite(RELE_PIN, HIGH);

  luzAcesa = false;
}

