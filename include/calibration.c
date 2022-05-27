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