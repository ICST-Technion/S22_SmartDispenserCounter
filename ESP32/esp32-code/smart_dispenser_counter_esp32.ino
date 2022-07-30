/*
  -------------------------------------------------------------------------------------
  HX711_ADC
  Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
  Olav Kallhovd sept2017
  -------------------------------------------------------------------------------------
*/

/*
   Settling time (number of samples) and data filtering can be adjusted in the config.h file
   For calibration and storing the calibration value in eeprom, see example file "Calibration.ino"

   The update() function checks for new data and starts the next conversion. In order to acheive maximum effective
   sample rate, update() should be called at least as often as the HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS.
   If you have other time consuming code running (i.e. a graphical LCD), consider calling update() from an interrupt routine,
   see example file "Read_1x_load_cell_interrupt_driven.ino".

   This is an example sketch on how to use this library
*/

#include <HX711_ADC.h>

#if defined(ESP8266) || defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

#include <string>
#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <ESP32Servo.h>

WiFiMulti wifiMulti;

Servo myservo1; // create servo object to control a servo
// 16 servo objects can be created on the ESP32

Servo myservo2;

int pos = 0; // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42
#if defined(ARDUINO_ESP32S2_DEV)
int servoPin1 = 22;
int servoPin2 = 18;
#else
int servoPin1 = 22;
int servoPin2 = 18;
#endif

#define MOTORA 0
#define MOTORB 1

#define SENSOR_PIN 35
#define BUZZER_PIN 13

int BUZZER_frequency = 262;
int motorAPin1 = 27;
int motorAPin2 = 26;
int enableAPin = 14;

int motorBPin1 = 32;
int motorBPin2 = 33;
int enableBPin = 25;

int BUZZER_channel = 1;

// Setting PWM properties
const int freq = 20000;
const int pwmChannel = 3;
const int resolution = 8;
int dutyCycle = 165;

// pins:
const int HX711_dout = 4; // mcu > HX711 dout pin
const int HX711_sck = 5;  // mcu > HX711 sck pin

// HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;
int counter = 0;
float prev = 0;
float base = 0;
bool got_base = false;
bool first_change = false;
float i;

float total_A = 0;

float total_B = 0;
float initial_weight = 0;
bool finishedA = false;
bool finishedB = false;
bool firstimehere = true;

int A_CLEAN = 17;
int A_DIRTY = 65;
int B_DIRTY = 125;
int B_CLEAN = 178;
int SCREW_REC = 120;
int done = false;

int lastState = LOW; // the previous state from the input pin
int currentState;    // the current reading from the input pin
bool turnedon = false;

void setup()
{

  Serial.begin(57600);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--)
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  wifiMulti.addAP("Mohbesher", "123123123M");

  myservo1.setPeriodHertz(50);
  myservo2.setPeriodHertz(50);
  // standard 50 hz servo
  myservo1.attach(servoPin1, 500, 2400);
  myservo2.attach(servoPin2, 500, 2400);
  // sets the pins as outputs:
  pinMode(motorAPin1, OUTPUT);
  pinMode(motorAPin2, OUTPUT);
  pinMode(enableAPin, OUTPUT);

  pinMode(motorBPin1, OUTPUT);
  pinMode(motorBPin2, OUTPUT);
  pinMode(enableBPin, OUTPUT);

  pinMode(SENSOR_PIN, INPUT);
  // pinMode(BUZZER_PIN, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enableAPin, pwmChannel);
  ledcAttachPin(enableBPin, pwmChannel);

  // testing
  Serial.print("Testing DC Motor...");

  delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  LoadCell.setReverseOutput(); // uncomment to turn a negative output value to positive
  float calibrationValue;      // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0;    // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266) || defined(ESP32)
  // EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  // EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true;                 // set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag())
  {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1)
      ;
  }
  else
  {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

void loop()
{
  int screw_count_A = 0;
  int screw_count_B = 0;

  if ((wifiMulti.run() == WL_CONNECTED))
  {

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    // http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
    http.begin("https://smart-dispenser-counter-server-rn4s6e52pa-uc.a.run.app/get_dispenser_job"); // HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK)
      {
        String payload = http.getString();

        DynamicJsonDocument myObject(2048);
        deserializeJson(myObject, payload);

        const char *name = myObject["requesterName"];
        Serial.println(name);

        screw_count_A = myObject["typeACount"];
        screw_count_B = myObject["typeBCount"];

        Serial.println(screw_count_A);
        Serial.println(screw_count_B);
      }
      else
      {
        delay(2000);
        return;
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      delay(2000);
      return;
    }

    http.end();
    bool true_weight_A = false;
    bool true_weight_B = false;

    delay(5000);

    if (!true_weight_A && !true_weight_B)
    {
      true_weight_A = in_action(MOTORA, screw_count_A);
    }

    if (true_weight_A && !true_weight_B)
    {
      delay(10000);
      done = false;
      true_weight_B = in_action(MOTORB, screw_count_B);
    }

    if (true_weight_A && true_weight_B)
    {
      Serial.print("stopped");
      dc_stop(motorAPin1, motorAPin2);
      dc_stop(motorBPin1, motorBPin2);
      done = false;
    }

    delay(2000);
  }
}

float find_base(int screw_count, int pin1, int pin2) // 3
{
  delay(3000);

  dc_move(pin1, pin2);

  float total = 0;
  i = LoadCell.getData();

  while (!first_change && !got_base)
  {
    prev = i;
    i = LoadCell.getData();
    initial_weight = i;

    LoadCell.update();

    Serial.print("Load_cell output val: ");
    Serial.println(i);

    if (i - prev > 0.2 && !first_change)
    {
      Serial.print("first change ");
      Serial.println(i);
      dc_stop(pin1, pin2);
      first_change = true;
    }

    if (first_change && !got_base)
    {
      while (!got_base)
      {
        dc_stop(pin1, pin2);
        prev = i;
        i = LoadCell.getData();

        LoadCell.update();
        Serial.print("Load_cell output val: ");
        Serial.println(i);

        if ((i - prev <= 0.02 && i - prev > 0) || (prev - i <= 0.02 && prev - i > 0))
        {
          got_base = true;
          base = i - initial_weight;
          Serial.print("base is: ");
          Serial.println(base);
          delay(1000);
          total = screw_count * base + initial_weight;
          Serial.println(total);
          delay(1000);
          delay(15);
          return total;
        }
      }
    }
  }
}

void dc_stop(int pin1, int pin2)
{
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

void dc_move(int pin1, int pin2)
{
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  ledcWrite(pwmChannel, dutyCycle);
}

void servo_move(int src, int dst, bool to_pour)
{
  if (src < dst)
  {
    for (int i = src; i <= dst; i += 1)
    {
      myservo1.write(i);
      delay(20);
    }
    if (to_pour)
    {

      pour();
    }
    for (int i = dst; i >= src; i -= 1)
    {
      myservo1.write(i);
      delay(20);
    }
    delay(2000);
  }
  else
  {
    for (int i = src; i >= dst; i -= 1)
    {
      myservo1.write(i);
      delay(20);
    }

    if (to_pour)
    {

      pour();
    }
    for (int i = dst; i <= src; i += 1)
    {
      myservo1.write(i);
      delay(20);
    }
    delay(2000);
  }
}

void pour()
{
  while (digitalRead(SENSOR_PIN))
  {
    Serial.println(digitalRead(SENSOR_PIN));
  }

  Serial.println(digitalRead(SENSOR_PIN));

  for (int pos2 = 10; pos2 <= 50; pos2 += 1)
  {
    myservo2.write(pos2);
    delay(30);
  }

  delay(2000);

  for (int pos2 = 50; pos2 >= 10; pos2 -= 1)
  {
    myservo2.write(pos2);
    delay(30);
  }
}

void something_went_wrong(int dirty_pos)
{
  Serial.print("something went wrong ");
  delay(5000);

  servo_move(SCREW_REC, dirty_pos, true);
  get_steady();
}

void reached_goal(int clean_pos)
{
  Serial.println("reached 7  ");
  delay(5000);

  servo_move(SCREW_REC, clean_pos, true);
  get_steady();
}

void get_steady()
{
  for (int j = 0; j < 70; j++)
  {
    delay(100);
    i = LoadCell.getData();
    LoadCell.update();

    Serial.print("get_ steady function ");
    Serial.println(i);
  }

  delay(500);
}

int in_action(int working_motor, float screw_count)
{
  bool first_time = true;
  bool finished = false;
  bool true_weight = false;
  float total = 0;
  float returned = 0;
  int clean;
  int dirty;
  int active_pin1 = 0;
  int active_pin2 = 0;
  int passive_pin1 = 0;
  int passive_pin2 = 0;

  if (working_motor == 0)
  {
    active_pin1 = motorAPin1;
    active_pin2 = motorAPin2;
    passive_pin1 = motorBPin1;
    passive_pin2 = motorBPin2;
    clean = A_CLEAN;
    dirty = A_DIRTY;
  }
  else
  {
    active_pin1 = motorBPin1;
    active_pin2 = motorBPin2;
    passive_pin1 = motorAPin1;
    passive_pin2 = motorAPin2;
    clean = B_CLEAN;
    dirty = B_DIRTY;
  }

  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; // increase value to slow down serial print activity
  // check for new data/start next conversion:
  if (LoadCell.update())
    newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady && !done)
  { // 1
    if (millis() > t + serialPrintInterval)
    {
      while (!done)
      {
        if (first_time)
        {
          get_steady();
          first_time = false;
        }

        dc_move(active_pin1, active_pin2); // 4
        dc_stop(passive_pin1, passive_pin2);

        prev = i;
        i = LoadCell.getData();

        LoadCell.update();
        Serial.print("Load_cell output val: ");
        Serial.println(i);

        if (first_change && got_base)
        {
          if (i - initial_weight > total + (0.3 * screw_count))
          {
            something_went_wrong(dirty);
            first_change = false;
            first_time = true;
            got_base = false;
            base = 0;
            prev = 0;
            i = 0;
            total = 0;
          } else {
            if (initial_weight < 0)
              initial_weight = -1 * initial_weight;

            if (i <= total + (0.3 * screw_count) && i >= total - (0.3 * screw_count))
            {
              dc_stop(active_pin1, active_pin2);
              get_steady();
              if (i > total + (0.5 * screw_count) || i < total - (0.5 * screw_count))
              {
                something_went_wrong(dirty);
                first_change = false;
                first_time = true;
                got_base = false;
                base = 0;
                prev = 0;
                i = 0;
                total = 0;
              } else {
                reached_goal(clean);

                done = true;
                true_weight = true;
                first_change = false;
                first_time = true;
                got_base = false;

                base = 0;
                i = 0;
                prev = 0;
                finished = true;
                total = 0;

                return true_weight;
              }
            }
          }
        }

        if (!finished)
        {
          if (!got_base && !first_change) // 8
            total = find_base(screw_count, active_pin1, active_pin2);
        }

        if (Serial.available() > 0)
        {
          char inByte = Serial.read();

          if (inByte == 't')
            LoadCell.tareNoDelay();
        }

        // check if last tare operation is complete:
        if (LoadCell.getTareStatus() == true)
        {
          Serial.println("Tare complete");
        }
      }
    }
  }
}

void doneAll()
{
  ledcAttachPin(BUZZER_PIN, 1);
  
  ledcWriteTone(1, 523);
  delay(100);
  ledcWriteTone(1, 1047);
  delay(100);
  ledcWriteTone(1, 2093);
  delay(100);
  ledcDetachPin(BUZZER_PIN);
  delay(5000);
}
