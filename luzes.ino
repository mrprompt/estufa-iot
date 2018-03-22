void controlaLampada()
{
  Serial.println("- Controlando luz");
  
  int horaAtual = timeClient.getHours();

  switch (horaAtual) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
      digitalWrite(RELE_PIN, LOW);

      luzAcesa = true;
      break;

    default:
      digitalWrite(RELE_PIN, HIGH);

      luzAcesa = false;
  }

  delay(200);
}
