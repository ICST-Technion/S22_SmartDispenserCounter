int motorAPin1 = 27;
int motorAPin2 = 26;
int enableAPin = 14;

int motorBPin1 = 13;
int motorBPin2 = 12;
int enableBPin = 15;

// Setting PWM properties
const int freq = 22000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 170;

void setup()
{
  Serial.begin(115200);
  // sets the pins as outputs:
  pinMode(motorAPin1, OUTPUT);
  pinMode(motorAPin2, OUTPUT);
  pinMode(enableAPin, OUTPUT);

  pinMode(motorBPin1, OUTPUT);
  pinMode(motorBPin2, OUTPUT);
  pinMode(enableBPin, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enableAPin, pwmChannel);
  ledcAttachPin(enableBPin, pwmChannel);

  // testing
  Serial.print("Testing DC Motor...");
}

void loop()
{
  // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, HIGH);
  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, HIGH);
  ledcWrite(pwmChannel, dutyCycle);
  delay(5000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, LOW);
  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, LOW);
  delay(1000);
}
