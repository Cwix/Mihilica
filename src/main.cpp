#include <Arduino.h>
#include "../include/header.h"

volatile int ch1_value = 0;
volatile int ch1_prev_time = 0;

volatile int ch2_value = 0;
volatile int ch2_prev_time = 0;

const int deadband = 50;
const int centerSignal = 1500;

/** @todo figure out calibration **/
int minInputSignal = 900;
int maxInputSignal = 2100;
 
int M1RPWM_Output = 5; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int M1LPWM_Output = 6; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)

int M2RPWM_Output = 10;
int M2LPWM_Output = 11;

void setup() 
{
  pinMode(M1RPWM_Output, OUTPUT);
  pinMode(M1LPWM_Output, OUTPUT);

  pinMode(M2RPWM_Output, OUTPUT);
  pinMode(M2LPWM_Output, OUTPUT);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(2), risingCh1, RISING);
  attachInterrupt(digitalPinToInterrupt(3), risingCh2, RISING);
}
 
void loop() 
{
  handleIdle();

  /** Motor 1 **/
  if(!isIdle(ch1_value)) {
    if (ch1_value > 1500)
    {
      int motor1Value = map(ch1_value, 1500, maxInputSignal, 0, 255);
      // reverse rotation
      analogWrite(M1LPWM_Output, 0);
      analogWrite(M1RPWM_Output, motor1Value);
    }
    else
    {
      int motor1Value = map(ch1_value, minInputSignal, 1500, 255, 0);
      // forward rotation
      analogWrite(M1RPWM_Output, 0);
      analogWrite(M1LPWM_Output, motor1Value);
    }
  }
  

  /** Motor 2 **/
  if(!isIdle(ch2_value)) {
    if (ch2_value > 1500)
    {
      int motor2Value = map(ch2_value, 1500, maxInputSignal, 0, 255);
      // reverse rotation
      analogWrite(M2LPWM_Output, 0);
      analogWrite(M2RPWM_Output, motor2Value);
    }
    else
    {
      int motor2Value = map(ch2_value, minInputSignal, 1500, 255, 0);
      // forward rotation
      analogWrite(M2RPWM_Output, 0);
      analogWrite(M2LPWM_Output, motor2Value);
    }
  }
  
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