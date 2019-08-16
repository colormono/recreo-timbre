/*
  Relay
  Prende, espera, apaga, loop

  Relay:
  * connect pin IN to Arduino pin D4
  * connect pin Vcc to Arduino pin 5V
  * connect pin GND to Arduino pin GND
*/

int relayPin = 4;

void setup()
{
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
}

void loop()
{
  delay(100);
  digitalWrite(relayPin, HIGH);
  delay(100);
  digitalWrite(relayPin, LOW);
  delay(3000);
}
