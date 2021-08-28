#include <Arduino.h>

void risingCh1();
void fallingCh1();

void risingCh2();
void fallingCh2();

volatile int ch1_value = 0;
volatile int ch1_prev_time = 0;

volatile int ch2_value = 0;
volatile int ch2_prev_time = 0;

int deadband = 50;
int centerSignal = 1500;
 
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

  Serial.begin(115200);
  // when pin D2 goes high, call the rising function
  attachInterrupt(digitalPinToInterrupt(2), risingCh1, RISING);
  attachInterrupt(digitalPinToInterrupt(3), risingCh2, RISING);
}

bool isIdle(int channelValue) 
{
  return channelValue > centerSignal - deadband && channelValue < centerSignal + deadband;
}
 
void loop() 
{
  if(isIdle(ch1_value)) 
  {
    analogWrite(M1RPWM_Output, 0);
    analogWrite(M1LPWM_Output, 0);
  }

  if (ch1_value > 1500)
  {
    int motor1Value = map(ch1_value, 1500, 2100, 0, 256);
    // reverse rotation
    Serial.print("Motor 1 reverse ");
    Serial.println(motor1Value);
    analogWrite(M1LPWM_Output, 0);
    analogWrite(M1RPWM_Output, motor1Value);
  }
  else
  {
    int motor1Value = map(ch1_value, 900, 1500, 256, 0);
    // forward rotation
    Serial.print("Motor 1 forward ");
    Serial.println(motor1Value);
    analogWrite(M1RPWM_Output, 0);
    analogWrite(M1LPWM_Output, motor1Value);
  }
}


//INTERRUPT ROUTINES
void risingCh1() {
  attachInterrupt(digitalPinToInterrupt(2), fallingCh1, FALLING);
  ch1_prev_time = micros();
}

void fallingCh1() {
  attachInterrupt(digitalPinToInterrupt(2), risingCh1, RISING);
  ch1_value = micros()-ch1_prev_time;
  // Serial.print("CH1: ");
  // Serial.println(ch1_value);
}

void risingCh2() {
  attachInterrupt(digitalPinToInterrupt(3), fallingCh2, FALLING);
  ch1_prev_time = micros();
}
 
void fallingCh2() {
  attachInterrupt(digitalPinToInterrupt(3), risingCh2, RISING);
  ch2_value = micros()-ch2_prev_time;
  Serial.print("CH2: ");
  Serial.println(ch2_value);
}