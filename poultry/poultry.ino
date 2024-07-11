#include <HX711.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(19,23,18,17,16,15);

const int LOADCELL_DOUT_PIN = 2; // Data output pin
const int LOADCELL_SCK_PIN = 3;  // Clock pin

HX711 scale;

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud
  lcd.begin(16,2);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // Initialize the scale with the specified pins
  lcd.print("Tare - Remove weights"); // Prompt to remove weights
  lcd.setCursor(0,1);
  delay(5000); // Wait for 5 seconds
  scale.tare(); // Reset the scale to 0
  lcd.clear();
  lcd.print("Tare done...");
  delay(2000);
  lcd.clear();
  lcd.print("Now put weight on the scale"); // Prompt to put weights
  lcd.setCursor(0,1);
  lcd.print("on the scale");
  delay(5000); // Wait for 5 seconds
}

void loop() {
  
  if (scale.is_ready()) {
    long reading = scale.get_units(10); // Get average of 10 readings to calculate weight
    float weight = reading / 419.8; // Divide by calibration factor to get the weight in desired units
    lcd.clear();
    lcd.print("Weight: ");
    lcd.print(weight, 2); // Print the weight with 2 decimal places
    lcd.print(" kg");
  } else {
    lcd.print("HX711 not found.");
  }
  delay(1000); // Delay before the next reading
}