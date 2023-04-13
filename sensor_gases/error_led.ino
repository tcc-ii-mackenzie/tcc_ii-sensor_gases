void errorLed(int delayTime) {
  digitalWrite(redLed, HIGH);
  delay(delayTime);
  digitalWrite(redLed, LOW);
  delay(delayTime);
}
