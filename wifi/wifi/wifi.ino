void loop() {
  char key = keypad.getKey();
  if (key){
    Serial.println(key);

if (settingWeight) {
      if (key == 'D') {
        // Finish weight input
        if (settingMode == 1) {
          weightThreshold1 = weightInput.toFloat();
          Serial.print("New minimum weight: ");
          Serial.println(weightThreshold1);
lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Min weight set");
