#include <HX711.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>


const int LOADCELL_DOUT_PIN = 2; // Data output pin
const int LOADCELL_SCK_PIN = 3;  // Clock pin

HX711 scale;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const float calibration_factor = 419.8;


void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud
  lcd.begin(16,2);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // Initialize the scale with the specified pins
  lcd.print("Tare - Remove weights"); // Prompt to remove weights
  lcd.setCursor(0,1);
  cd.print("weights");
  Serial.println("Tare - Remove weights");
  delay(5000); // Wait for 5 seconds
  scale.tare(); // Reset the scale to 0
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tare done...");
  Serial.println("Tare done...");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Put weight on");
  lcd.setCursor(0, 1);
  lcd.print("the scale");
  lcd.print("Now put weight on the scale"); // Prompt to put weights
  delay(5000); // Wait for 5 seconds
  lcd.clear();
}

void loop() {
  
  if (scale.is_ready()) {
    long reading = scale.get_units(10); // Get average of 10 readings to calculate weight
    float weight = reading / calibration_factor; // Divide by calibration factor to get the weight in desired units
    Serial.print("Weight: ");
    Serial.print(weight, 2); // Print the weight with 2 decimal places
    Serial.println(" kg");

    lcd.setCursor(0, 0);
    lcd.print("Weight: ");
    lcd.setCursor(0, 1);
    lcd.print(weight, 2); // Print the weight on the LCD with 2 decimal places
    lcd.print(" kg");
  } else {
    Serial.println("HX711 not found.");
    lcd.setCursor(0, 0);
    lcd.print("HX711 not found");
  }
  delay(1000); // Delay before the next reading
}
