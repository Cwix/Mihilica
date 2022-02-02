#include <Arduino.h>
#include "../include/header.h"

//CH1 and CH2 are RC values (1000-2000) so we set them at 1500 to zero them out
volatile int ch1_value = 1500;
volatile int ch1_prev_time = 0;

volatile int ch2_value = 1500;
volatile int ch2_prev_time = 0;

//These are analogRead values (0-1024) so we set them at 0 to zero them out
volatile int ch3_value = 0;
volatile int ch4_value = 0;

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
const int CH3 = 23;
const int CH4 = 24;

const int ENGINE_ENABLE_OUT = 7;
const int STARTER_OUTPUT = 8;

boolean engineEnabled = 0;

void setup() 
{
  Serial.begin(115200);

  pinMode(M1RPWM_OUTPUT, OUTPUT);
  pinMode(M1LPWM_OUTPUT, OUTPUT);

  pinMode(M2RPWM_OUTPUT, OUTPUT);
  pinMode(M2LPWM_OUTPUT, OUTPUT);

  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);

  pinMode(ENGINE_ENABLE_OUT, OUTPUT);
  pinMode(STARTER_OUTPUT, OUTPUT);

  digitalWrite(ENGINE_ENABLE_OUT, engineEnabled);
  digitalWrite(STARTER_OUTPUT, LOW);

  attachInterrupt(digitalPinToInterrupt(2), risingCh1, RISING);
  attachInterrupt(digitalPinToInterrupt(3), risingCh2, RISING);

  Serial.println(F("Mihilica ready."));

  // calibrate();
}
 
void loop() 
{
  Serial.print("CH1 val: ");
  Serial.println(ch1_value);

  Serial.print("CH2 val: ");
  Serial.println(ch2_value);

  Serial.print("CH3 val: ");
  Serial.println(analogRead(CH3));

  Serial.print("CH4 val: ");
  Serial.println(analogRead(CH4));

  handleIdle();

  if(analogRead(CH3) > 700) {
      engineEnabled = 1;
      digitalWrite(ENGINE_ENABLE_OUT, HIGH);
  } else {
      engineEnabled = 0;
      digitalWrite(ENGINE_ENABLE_OUT, LOW);
  }


  if(analogRead(CH4) > 700) {
    digitalWrite(STARTER_OUTPUT, HIGH);
  } else {
    digitalWrite(STARTER_OUTPUT, LOW);
  }


  if(engineEnabled) {
    /** Motor 1 **/
    if(!isIdle(ch1_value)) {
      if (ch1_value > 1500)
      {
        int motor1Value = map(ch1_value, 1500, ch1MaxInputSignal, 0, 255);
        // reverse rotation
        analogWrite(M1LPWM_OUTPUT, 0);
        analogWrite(M1RPWM_OUTPUT, motor1Value);
      }
      else
      {
        int motor1Value = map(ch1_value, ch1MinInputSignal, 1500, 255, 0);
        // forward rotation
        analogWrite(M1RPWM_OUTPUT, 0);
        analogWrite(M1LPWM_OUTPUT, motor1Value);
      }
    }

    /** Motor 2 **/
  if(!isIdle(ch2_value)) {
    if (ch2_value > 1500)
    {
      int motor2Value = map(ch2_value, 1500, ch2MaxInputSignal, 0, 255);
      // reverse rotation
      analogWrite(M2LPWM_OUTPUT, 0);
      analogWrite(M2RPWM_OUTPUT, motor2Value);
    }
    else
    {
      int motor2Value = map(ch2_value, ch2MinInputSignal, 1500, 255, 0);
      // forward rotation
      analogWrite(M2RPWM_OUTPUT, 0);
      analogWrite(M2LPWM_OUTPUT, motor2Value);
    }
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

  Serial.print(F("Channel 1 min: "));
  Serial.println(ch1MinInputSignal);
  Serial.print(F("Channel 1 max: "));
  Serial.println(ch1MaxInputSignal);

  Serial.print(F("Channel 2 min: "));
  Serial.println(ch2MinInputSignal);
  Serial.print(F("Channel 2 max: "));
  Serial.println(ch2MaxInputSignal);

  // signal the end of the calibration period
  digitalWrite(13, LOW);
}

bool isIdle(int channelValue) 
{
  return channelValue > (centerSignal - deadband) && channelValue < (centerSignal + deadband);
}

/** 
 * Reset values to 0 if sticks are in idle deadband
 **/
void handleIdle() {
  if(isIdle(ch1_value)) 
  {
    analogWrite(M1RPWM_OUTPUT, 0);
    analogWrite(M1LPWM_OUTPUT, 0);
  }

  if(isIdle(ch2_value)) 
  {
    analogWrite(M2RPWM_OUTPUT, 0);
    analogWrite(M2LPWM_OUTPUT, 0);
  }
}

/******
 * 
 * INTERRUPT ROUTINES
 * 
 *****/
void risingCh1() {
  attachInterrupt(digitalPinToInterrupt(CH1), fallingCh1, FALLING);
  ch1_prev_time = micros();
}

void fallingCh1() {
  attachInterrupt(digitalPinToInterrupt(CH1), risingCh1, RISING);
  ch1_value = micros() - ch1_prev_time;
}

void risingCh2() {
  attachInterrupt(digitalPinToInterrupt(CH2), fallingCh2, FALLING);
  ch1_prev_time = micros();
}
 
void fallingCh2() {
  attachInterrupt(digitalPinToInterrupt(CH2), risingCh2, RISING);
  ch2_value = micros() - ch2_prev_time;
}