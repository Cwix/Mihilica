#include <Arduino.h>
#include "../include/header.h"

//CH1 and CH2 are RC values (1000-2000) so we set them at 1500 to zero them out
volatile int ch1Value = 1500;
volatile int ch2Value = 1500;

volatile bool ch3Value = 0;
volatile bool ch4Value = 0;

const int deadband = 50;
const int centerSignal = 1500;

/**
 * Some sane values for startup
 * 
 * @todo figure out calibration 
 * **/
int ch1MinInputSignal = 900;
int ch1MaxInputSignal = 2100;

int ch2MinInputSignal = 900;
int ch2MaxInputSignal = 2100;

//PIN Configurations
const int M1RPWM_OUTPUT = 5; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
const int M1LPWM_OUTPUT = 6; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)

const int M2RPWM_OUTPUT = 10;
const int M2LPWM_OUTPUT = 11;

const int CH1 = 2;
const int CH2 = 3;
const int CH3 = A0;
const int CH4 = A1;

const int ENGINE_ENABLE_OUT = 7;
const int STARTER_OUTPUT = 8;

void setup() 
{
  Serial.begin(115200);

  pinMode(M1RPWM_OUTPUT, OUTPUT);
  pinMode(M1LPWM_OUTPUT, OUTPUT);

  pinMode(M2RPWM_OUTPUT, OUTPUT);
  pinMode(M2LPWM_OUTPUT, OUTPUT);

  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);

  pinMode(ENGINE_ENABLE_OUT, OUTPUT);
  pinMode(STARTER_OUTPUT, OUTPUT);

  digitalWrite(ENGINE_ENABLE_OUT, LOW);
  digitalWrite(STARTER_OUTPUT, LOW);

  Serial.println(F("Mihilica ready."));

  // calibrate();
}

void loop() 
{

  ch1Value = readChannel(CH1, ch1MinInputSignal, ch2MaxInputSignal, centerSignal);
  ch2Value = readChannel(CH2, ch2MinInputSignal, ch2MaxInputSignal, centerSignal);
  ch3Value = readSwitch(CH3, false);
  ch4Value = readSwitch(CH4, false);

  if(!ch3Value) {
      digitalWrite(ENGINE_ENABLE_OUT, HIGH);
  } else {
      digitalWrite(ENGINE_ENABLE_OUT, LOW);
  }

  if(!ch4Value) {
    digitalWrite(STARTER_OUTPUT, HIGH);
  } else {
    digitalWrite(STARTER_OUTPUT, LOW);
  }

  int motor1Value;
  /** Motor 1 **/

  if (ch1Value > centerSignal)
  {
    motor1Value = map(ch1Value, centerSignal, ch1MaxInputSignal, 0, 255);

    if(isIdle(ch1Value)) {
      motor1Value = 0;
    }
    
    // reverse rotation
    analogWrite(M1LPWM_OUTPUT, 0);
    analogWrite(M1RPWM_OUTPUT, motor1Value);
  }
  else
  {
    motor1Value = map(ch1Value, ch1MinInputSignal, centerSignal, 255, 0);
    
    if(isIdle(ch1Value)) {
      motor1Value = 0;
    }
    // forward rotation
    analogWrite(M1RPWM_OUTPUT, 0);
    analogWrite(M1LPWM_OUTPUT, motor1Value);
  }


  int motor2Value;
  /** Motor 2 **/

  if (ch2Value > centerSignal)
  {
    motor2Value = map(ch2Value, centerSignal, ch2MaxInputSignal, 0, 255);
    if(isIdle(ch2Value)) {
    motor2Value = 0;
  }
    // reverse rotation
    analogWrite(M2LPWM_OUTPUT, 0);
    analogWrite(M2RPWM_OUTPUT, motor2Value);
  }
  else
  {
    motor2Value = map(ch2Value, ch2MinInputSignal, centerSignal, 255, 0);
    if(isIdle(ch2Value)) {
      motor2Value = 0;
    }
    // forward rotation
    analogWrite(M2RPWM_OUTPUT, 0);
    analogWrite(M2LPWM_OUTPUT, motor2Value);
  }
}

void calibrate() {
  // turn on LED to signal the start of the calibration period:
  // pinMode(13, OUTPUT);
  // digitalWrite(13, HIGH);

  // calibrate during the first five seconds
  while (millis() < 5000) {
    Serial.println(ch1Value);
    // record the maximum sensor value
    if(ch1Value > 800 && ch1Value < 2500) { //Sanity check
      if (ch1Value > ch1MaxInputSignal) {
      ch1MaxInputSignal = ch1Value;
      }

      // record the minimum sensor value
      if (ch1Value < ch1MinInputSignal) {
        ch1MinInputSignal = ch1Value;
      }
    }

    // record the maximum sensor value
    if (ch2Value > ch2MaxInputSignal) {
      ch2MaxInputSignal = ch2Value;
    }

    // record the minimum sensor value
    if (ch2Value < ch2MinInputSignal) {
      ch2MinInputSignal = ch2Value;
    }
  }

  Serial.print(F("Channel 1 min: "));
  Serial.println(ch1MinInputSignal);
  Serial.print(F("Channel 1 max: "));
  Serial.println(ch1MaxInputSignal);

  Serial.print(F("Channel 2 min: "));
  Serial.println(ch2MinInputSignal);
  Serial.print(F("Channel 2 max: "));
  Serial.println(ch2MaxInputSignal);

  // signal the end of the calibration period
  // digitalWrite(13, LOW);
}

bool isIdle(int channelValue) 
{
  return channelValue > (centerSignal - deadband) && channelValue < (centerSignal + deadband);
}

// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 30000);
  
  //ch must be between ~1000-2000 for servo signals
  if (ch < 100) return defaultValue;
  
  return constrain(ch, minLimit, maxLimit);
}

// Red the channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue){
  int intDefaultValue = (defaultValue) ? 100 : 0;
  
  int ch = readChannel(channelInput, 1000, 2000, intDefaultValue);

  return (ch > centerSignal);
}
