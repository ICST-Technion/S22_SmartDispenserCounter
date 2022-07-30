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
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

int motorAPin1 = 27; 
int motorAPin2 = 26; 
int enableAPin = 14; 


int motorBPin1 = 13; 
int motorBPin2 = 12; 
int enableBPin = 15; 



// Setting PWM properties
const int freq = 20000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 170;


//pins:
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;
int counter = 0;
float prev = 0;
float base = 0;
bool got_base = false;
bool first_change = false;
float i;
bool true_weight_A = false;
int desired_value_A = 7;
float desiredA = 0;
bool true_weight_B = false;
int desired_value_B = 5;
float desiredB = 0;

bool finishedA = false ; 
bool finishedB = false ; 


void setup() {
 Serial.begin(57600);
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

  
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266)|| defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

void loop() {
  
  if(!true_weight_A){
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, HIGH);
  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, LOW); 
  ledcWrite(pwmChannel, dutyCycle);  
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      prev = i;
      i = LoadCell.getData();
      if(first_change && got_base)
      {
        delay(100);
        if(i <= desiredA +(0.2*desired_value_A) && i >= desiredA -(0.2*desired_value_A))
        {
          digitalWrite(motorAPin1, LOW);
          digitalWrite(motorAPin2,LOW);
          Serial.print("reached 7  ");
           delay(100);
          true_weight_A = true;
           first_change = false;
           got_base = false ; 
           base = 0 ;
           i = 0;
           prev = 0;
           finishedA = true ;
           LoadCell.tare();
           delay(30000);
        }
      }
      }
      if(!finishedA){
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      if((i - prev > 0.2 || prev - i > 0.2) && !first_change){
        first_change = true;
      }
      if(first_change && !got_base){
        delay(100);
        while(!got_base && LoadCell.update()){
          prev = i;
          i = LoadCell.getData();
          Serial.print("Load_cell output val: ");
          Serial.println(i);
          if((i - prev <= 0.02 && i - prev > 0) || (prev - i <= 0.02 && prev - i > 0)){
            got_base = true;
            base = i;
            Serial.print("base is: ");
            Serial.println(base);
            counter++;
            desiredA = desired_value_A*base;
            delay(15);
            break;
          }
        }
      }
    if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
  }
  }
  }
    /* if(got_base && LoadCell.update()){
        delay(100);
        if(i - counter*base < base && i - counter*base > 1 || counter*base - i < base && counter*base - i > 1){
          counter++;
        }
        else if(i - counter*base > base){
          Serial.println("something went wrong");
          delay(50000);
        }
      }
      if(counter == desired_value_A){
        delay(100);
        Serial.println("done");
        delay(50000);
      }
      newDataReady = 0;
      t = millis();
    }
  }*/
  else if(!true_weight_B){
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, LOW);
  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, HIGH); 
  ledcWrite(pwmChannel, dutyCycle);  
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      prev = i;
      i = LoadCell.getData();
      if(first_change && got_base)
      {
        delay(100);
        if(i <= desiredB +(0.2*desired_value_B) && i >= desiredB -(0.2*desired_value_B))
        {
          Serial.print("reached 5  ");
          true_weight_B = true;
          digitalWrite(motorBPin1, LOW);
          digitalWrite(motorBPin2, LOW);
        }
      }
      }
      if(!true_weight_B){
      Serial.print("Load_cell output val: ");
      Serial.println(i);
      if((i - prev > 0.2 || prev - i > 0.2) && !first_change){
        first_change = true;
      }
      if(first_change && !got_base){
        delay(100);
        while(!got_base && LoadCell.update()){
          prev = i;
          i = LoadCell.getData();
          Serial.print("Load_cell output val: ");
          Serial.println(i);
          if((i - prev <= 0.02 && i - prev > 0) || (prev - i <= 0.02 && prev - i > 0)){
            got_base = true;
            base = i;
            Serial.print("base is: ");
            Serial.println(base);
            counter++;
            desiredB = desired_value_B*base;
            delay(15);
            break;
          }
        }
      } 
  
      if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
      }
    }
  }
 
  
 

  

  // receive command from serial terminal, send 't' to initiate tare operation:

   digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, LOW);
  digitalWrite(motorBPin1, LOW);
  digitalWrite(motorBPin2, LOW);

}
