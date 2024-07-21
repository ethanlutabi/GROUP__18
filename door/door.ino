#include <ESP32Servo.h>
#include "HX711.h"

#define SERVO_PIN 4
#define SECOND_SERVO_PIN 2
#define RED_LED_PIN 13
#define GREEN_LED_PIN 12
#define LOADCELL_DOUT_PIN 18
#define LOADCELL_SCK_PIN 19
#define BUTTON_PIN 5

HX711 scale;
Servo servoMotor;
Servo secondServoMotor;

float weight;
bool firstDoorOpen = false;
bool secondDoorOpen = false;
bool systemEnabled = false;
bool lastButtonState = HIGH;

bool settingWeight = false;
String weightInput = "";
int settingMode = 0;
unsigned long menuStartTime = 0;
bool inMenu = false;

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

int ledMode = 0;
float weightThreshold1 = 1.0; // Default threshold for the first door
float weightThreshold2 = 2.5; // Default threshold for the second door

int firstDoorOpenCount = 0; // Count of first door openings
int secondDoorOpenCount = 0; // Count of second door openings
float previousWeight = 0.0; // Store previous weight to detect new load
bool lastSteadyPressureCounted = false; // Flag to track last steady pressure
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
  Serial.begin(115200); // ESP32 typically uses a higher baud rate
  Serial.println("Starting...");
    Serial.print(".");
    delay(1000);

  servoMotor.attach(SERVO_PIN);
  secondServoMotor.attach(SECOND_SERVO_PIN);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up resistor



 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(); // Adjust to this calibration factor
  scale.tare(); // Reset the scale to 0

  Serial.println("Setup complete");
}

void loop() {
  if (systemEnabled && !inMenu) {
    if (scale.is_ready()) {
      long reading = scale.get_units(10); // Get average of 10 readings to calculate weight
      float weight = reading / 419.8; // Get the average of 10 readings
      Serial.print("Weight: ");
      Serial.print(weight, 2); // Print the weight with 2 decimal places
      Serial.println(" kg");

      // lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Weight: ");
      lcd.print(weight, 2); // Print weight with 2 decimal places
      lcd.print(" kg");

      bool weightChanged = (fabs(weight - previousWeight) >= 0.1); // Change threshold (e.g., 0.1 kg)

      // Weight below minimum: Close both doors and turn off LEDs
      if (weight < weightThreshold1) {
        servoMotor.write(0); // Close first door
        firstDoorOpen = false;
        secondServoMotor.write(0); // Close second door
        secondDoorOpen = false;
        setLED(0); // Turn off LEDs

      } 
      // Weight between minimum and average: Open the first door, turn on first LED, and turn off second LED
      else if (weight >= weightThreshold1 && weight < weightThreshold2) {
        if (!firstDoorOpen || weightChanged) {
          servoMotor.write(90); // Open first door
          firstDoorOpen = true;
          secondServoMotor.write(0); // Ensure second door is closed
          secondDoorOpen = false;
          setLED(1); // Red glow
        }
      } 
      // Weight above average: Open the second door, turn off first LED, and turn on second LED
      else if (weight >= weightThreshold2) {
        if (!secondDoorOpen || weightChanged) {
          secondServoMotor.write(90); // Open second door
          secondDoorOpen = true;
          servoMotor.write(0); // Ensure first door is closed
          firstDoorOpen = false;
          setLED(2); // Green glow
      }
    } else {
      Serial.println("HX711 not found.");
      lcd.setCursor(0, 0);
      lcd.print("HX711 not found");
      setLED(0);
    }
  } else {
    // Ensure LED is off when the system is disabled
    setLED(0);
  }
  delay(100); // Adjust delay according to your needs

