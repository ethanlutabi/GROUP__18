#include <HX711.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>


const int LOADCELL_DOUT_PIN = 2; // Data output pin
const int LOADCELL_SCK_PIN = 3;  // Clock pin
const int RED_LED_PIN = 7;
const int GREEN_LED_PIN = 6;

HX711 scale;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const float calibration_factor = 419.8;

int ledMode = 0;

// Function to set LED mode
void setLED(int mode = -1) {
  // 0 = none, 1 = red-glow, 2 = green-glow
  if (mode >= 0 && mode <= 2) {
    ledMode = mode;
  }

  switch (ledMode) {
    case 0: // Turn off all LEDs
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
      break;
    case 1: // Red glow
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
      break;
    case 2: // Green glow
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, HIGH);
      break;
  }
}

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // Initialize the scale with the specified pins
  lcd.begin(16, 2); // Initialize the LCD
  lcd.backlight(); // Turn on the LCD backlight

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Tare - Remove");
  lcd.setCursor(0, 1);
  lcd.print("weights");
  Serial.println("Tare - Remove weights"); // Prompt to remove weights
  delay(5000); // Wait for 5 seconds
  scale.tare(); // Reset the scale to 0
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tare done...");
  Serial.println("Tare done...");
  delay(2000); // Wait for 2 seconds
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Put weight on");
  lcd.setCursor(0, 1);
  lcd.print("the scale");
  Serial.println("Now put weight on the scale"); // Prompt to put weights
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

    if (weight < 2.5) {
      setLED(1); // Red glow
    } else {
      setLED(2); // Green glow
    }

  } else {
    Serial.println("HX711 not found.");
    lcd.setCursor(0, 0);
    lcd.print("HX711 not found");
  }
  delay(1000); // Delay before the next reading
}
