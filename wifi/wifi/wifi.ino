
unsigned long myChannelNumber = 2606703;
const char* myWriteAPIKey = "MEAYCUGPIIB31FF4";

void loop() 
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
  } else if (key == 'C') {
        // Clear weight input
        weightInput = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(settingMode == 1 ? "Enter min weight" : "Enter avg weight");
      } else if (key == '#') {
// Remove last character
        if (weightInput.length() > 0) {
          weightInput.remove(weightInput.length() - 1);
          lcd.setCursor(0, 1);
          lcd.print(weightInput + " ");
        }
// Detect weight change and count door openings
      bool weightChanged = (fabs(weight - previousWeight) >= 0.1); // Change threshold (e.g., 0.1 kg)

      // Weight below minimum: Close both doors and turn off LEDs
      if (weight < weightThreshold1) {
        servoMotor.write(0); // Close first door
        firstDoorOpen = false;
        secondServoMotor.write(0); // Close second door
        secondDoorOpen = false;
        setLED(0); // Turn off LEDs
      } 
// Weight between minimum and average: Open the first door
      else if (weight >= weightThreshold1 && weight < weightThreshold2) {
        if (!firstDoorOpen) {
          servoMotor.write(90); // Open first door
          firstDoorOpen = true;
          firstDoorOpenCount++; // Increment first door open count
        }
        secondServoMotor.write(0); // Ensure second door is closed
        secondDoorOpen = false;
        setLED(1); // Turn on red LED
      } 
