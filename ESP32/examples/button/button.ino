#define BUTTON_PIN 19 // GIOP21 pin connected to button
int counter = 0;
// Variables will change:
int lastState = LOW; // the previous state from the input pin
int currentState;    // the current reading from the input pin
bool turnedon = false;
void setup()
{
  Serial.begin(9600);
  // initialize the pushbutton pin as an pull-up input
  pinMode(BUTTON_PIN, OUTPUT);
}

void loop()
{

  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);

  if (currentState == HIGH && lastState == LOW)
  {
    if (turnedon)
      turnedon = false;
    else
      turnedon = true;
  }
  Serial.println(turnedon);

  // save the last state
  lastState = currentState;
}
