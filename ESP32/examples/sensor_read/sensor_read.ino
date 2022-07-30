int pin = 35;
float value = 0 ;
float digitalvalue = 0 ;

void setup() {
  Serial.begin(57600);
  // put your setup code here, to run once:
  pinMode(pin, INPUT);
}

void loop() {
  delay(1000);
  while (! digitalRead(pin))
  {
    Serial.println(digitalRead(pin));
  }
  delay(2000);
  Serial.println(digitalRead(pin));
  // put your main code here, to run repeatedly:

  Serial.println(value);
  // Serial.println( digitalvalue);
}
