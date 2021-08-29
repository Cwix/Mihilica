#include <Arduino.h>
#include "../include/header.h"

volatile int ch1_value = 0;
volatile int ch1_prev_time = 0;

volatile int ch2_value = 0;
volatile int ch2_prev_time = 0;

const int deadband = 50;
const int centerSignal = 1500;

/** @todo figure out calibration **/
int ch1MinInputSignal = 900;
int ch1MaxInputSignal = 2100;

int ch2MinInputSignal = 900;
int ch2MaxInputSignal = 2100;
 
int M1RPWM_Output = 5; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int M1LPWM_Output = 6; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)

int M2RPWM_Output = 10;
int M2LPWM_Output = 11;

void setup() 
{
  Serial.begin(115200);

  pinMode(M1RPWM_Output, OUTPUT);
  pinMode(M1LPWM_Output, OUTPUT);

  pinMode(M2RPWM_Output, OUTPUT);
  pinMode(M2LPWM_Output, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(2), risingCh1, RISING);
  attachInterrupt(digitalPinToInterrupt(3), risingCh2, RISING);

  // calibrate();
}
 
void loop() 
{
  // Serial.print("Channel 1 val: ");
  // Serial.println(ch1_value);

  handleIdle();

  /** Motor 1 **/
  if(!isIdle(ch1_value)) {
    if (ch1_value > 1500)
    {
      int motor1Value = map(ch1_value, 1500, ch1MaxInputSignal, 0, 255);
      // reverse rotation
      analogWrite(M1LPWM_Output, 0);
      analogWrite(M1RPWM_Output, motor1Value);
    }
    else
    {
      int motor1Value = map(ch1_value, ch1MinInputSignal, 1500, 255, 0);
      // forward rotation
      analogWrite(M1RPWM_Output, 0);
      analogWrite(M1LPWM_Output, motor1Value);
    }
  }
  

  /** Motor 2 **/
  if(!isIdle(ch2_value)) {
    if (ch2_value > 1500)
    {
      int motor2Value = map(ch2_value, 1500, ch2MaxInputSignal, 0, 255);
      // reverse rotation
      analogWrite(M2LPWM_Output, 0);
      analogWrite(M2RPWM_Output, motor2Value);
    }
    else
    {
      int motor2Value = map(ch2_value, ch2MinInputSignal, 1500, 255, 0);
      // forward rotation
      analogWrite(M2RPWM_Output, 0);
      analogWrite(M2LPWM_Output, motor2Value);
    }
  }
  
}

void calibrate() {
  // turn on LED to signal the start of the calibration period:
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // calibrate during the first five seconds
  while (millis() < 5000) {
    Serial.println(ch1_value);
    // record the maximum sensor value
    if(ch1_value > 800 && ch1_value < 2500) { //Sanity check
      if (ch1_value > ch1MaxInputSignal) {
      ch1MaxInputSignal = ch1_value;
      }

      // record the minimum sensor value
      if (ch1_value < ch1MinInputSignal) {
        ch1MinInputSignal = ch1_value;
      }
    }

    // record the maximum sensor value
    if (ch2_value > ch2MaxInputSignal) {
      ch2MaxInputSignal = ch2_value;
    }

    // record the minimum sensor value
    if (ch2_value < ch2MinInputSignal) {
      ch2MinInputSignal = ch2_value;
    }
  }

  Serial.print("Channel 1 min: ");
  Serial.println(ch1MinInputSignal);
  Serial.print("Channel 1 max: ");
  Serial.println(ch1MaxInputSignal);

  Serial.print("Channel 2 min: ");
  Serial.println(ch2MinInputSignal);
  Serial.print("Channel 2 max: ");
  Serial.println(ch2MaxInputSignal);

  // signal the end of the calibration period
  digitalWrite(13, LOW);
}

bool isIdle(int channelValue) 
{
  return channelValue > centerSignal - deadband && channelValue < centerSignal + deadband;
}

/** 
 * Reset values to 0 if sticks are in idle deadband
 **/
void handleIdle() {
  if(isIdle(ch1_value)) 
  {
    analogWrite(M1RPWM_Output, 0);
    analogWrite(M1LPWM_Output, 0);
  }

  if(isIdle(ch2_value)) 
  {
    analogWrite(M2RPWM_Output, 0);
    analogWrite(M2LPWM_Output, 0);
  }
}

/******
 * 
 * INTERRUPT ROUTINES
 * 
 *****/
void risingCh1() {
  attachInterrupt(digitalPinToInterrupt(2), fallingCh1, FALLING);
  ch1_prev_time = micros();
}

void fallingCh1() {
  attachInterrupt(digitalPinToInterrupt(2), risingCh1, RISING);
  ch1_value = micros() - ch1_prev_time;
}

void risingCh2() {
  attachInterrupt(digitalPinToInterrupt(3), fallingCh2, FALLING);
  ch1_prev_time = micros();
}
 
void fallingCh2() {
  attachInterrupt(digitalPinToInterrupt(3), risingCh2, RISING);
  ch2_value = micros() - ch2_prev_time;
}