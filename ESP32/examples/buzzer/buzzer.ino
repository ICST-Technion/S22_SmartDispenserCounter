

int pin = 13;
// int channel = 10;
// int frequency = 262;

void setup()
{
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
}

void loop()
{
  digitalWrite(pin, HIGH);
  delay(1000);
  // digitalWrite(pin,LOW);
  delay(1000);
}

/*void cup_not_exist()
{
 ledcAttachPin(pin, channel);
  ledcWriteTone(channel, frequency);
  delay(500);
  ledcDetachPin(pin);
 // digitalWrite(pin, LOW);
  delay(500);

}*/
