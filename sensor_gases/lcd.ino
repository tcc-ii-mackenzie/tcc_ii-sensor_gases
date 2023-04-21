void lcdInitializeMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando J3M");
}

void lcdConfigMessage(String message) {
  lcdInitializeMessage();
  lcd.setCursor(0, 1);
  lcd.print(message);
}

void lcdLoopMessage(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("J3M");
  lcd.setCursor(0, 1);
  lcd.print(message);
}
