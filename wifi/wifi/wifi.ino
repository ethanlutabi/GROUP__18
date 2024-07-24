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
 } else if (settingMode == 2) {
          weightThreshold2 = weightInput.toFloat();
Serial.print("New average weight: ");
          Serial.println(weightThreshold2);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Avg weight set");
        }
delay(2000); // Wait for 2 seconds
        lcd.clear();
        settingWeight = false;
        settingMode = 0;
        inMenu = false; // Exit menu mode
