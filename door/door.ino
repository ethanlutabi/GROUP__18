#include <ESP32Servo.h>
#include "HX711.h"
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

#define SERVO_PIN 4
#define SECOND_SERVO_PIN 2
#define RED_LED_PIN 13
#define GREEN_LED_PIN 12
#define LOADCELL_DOUT_PIN 18
#define LOADCELL_SCK_PIN 19
#define BUTTON_PIN 5
const char* ssid = "Wokwi-GUEST";
const char* password="";

HX711 scale;
Servo servoMotor;
Servo secondServoMotor;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change the address if needed

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
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {40, 39, 38, 37}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {36, 35, 34, 33}; // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

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

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  servoMotor.attach(SERVO_PIN);
  secondServoMotor.attach(SECOND_SERVO_PIN);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up resistor

  Wire.begin(8, 9); // Set I2C pins for ESP32-S2 Mini (SDA, SCL)

  lcd.begin(16, 2); // Initialize the LCD
  lcd.backlight(); // Turn on the LCD backlight


  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(); // Adjust to this calibration factor
  scale.tare(); // Reset the scale to 0

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

    Serial.println("Setup complete");
}

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
        }
         else if (settingMode == 2) {
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
        else if (key == '*') {
        // Add decimal point
        if (weightInput.indexOf('.') == -1) {
          weightInput += ".";
          lcd.setCursor(0, 1);
          lcd.print(weightInput);
           }
      } else {
        // Continue weight input
        weightInput += key;
        lcd.setCursor(0, 1);
         lcd.print(weightInput);
      }
    } else {
      if (key == 'A') {
        // Open menu
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("1: Min Weight");
                lcd.setCursor(0, 1);
        lcd.print("2: Avg Weight");
        menuStartTime = millis(); // Record the time when menu was opened
           inMenu = true;
      } else if ((key == '1' || key == '2') && inMenu) {
        // Start setting weight
          settingWeight = true;
        settingMode = (key == '1') ? 1 : 2;
        weightInput = "";
        lcd.clear();
          lcd.setCursor(0, 0);
        lcd.print(settingMode == 1 ? "Enter min weight" : "Enter avg weight");
          } else if (key == 'B' && inMenu) {
        // Exit menu
        lcd.clear();
        inMenu = false;
        settingWeight = false;
        settingMode = 0;
      }
    }
  }

if (inMenu && (millis() - menuStartTime >= 20000)) {
    // Exit menu mode after 20 seconds
    lcd.clear();
    inMenu = false;
    settingWeight = false;
    settingMode = 0;
  }
   // Check if the button is pressed
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH)
   systemEnabled = !systemEnabled; // Toggle system state
    Serial.print("System ");
    Serial.println(systemEnabled ? "Enabled" : "Disabled");
    delay(500); // Debounce delay
    }
  lastButtonState = buttonState;
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
          // Detect weight change and count door openings
      bool weightChanged = (fabs(weight - previousWeight) >= 0.1); // Change threshold (e.g., 0.1 kg)

      // Weight below minimum: Close both doors and turn off LEDs
        if (weight < weightThreshold1) {
        servoMotor.write(0); // Close first door
        firstDoorOpen = false;
        secondServoMotor.write(0); // Close second door
        secondDoorOpen = false;
        setLED(0); // Turn off LEDs

            // Reset the flag for last steady pressure
        lastSteadyPressureCounted = false;
      } 
      // Weight between minimum and average: Open the first door, turn on first LED, and turn off second LED
       else if (weight >= weightThreshold1 && weight < weightThreshold2) {
        if (!firstDoorOpen || weightChanged) {
          servoMotor.write(90); // Open first door
          firstDoorOpen = true;
          secondServoMotor.write(0); // Ensure second door is closed
           secondDoorOpen = false;
          setLED(1); // Red glow
               // Increment first door open counter
          firstDoorOpenCount++;

          // Update flag for last steady pressure
          lastSteadyPressureCounted = true; 
        }
      } 
























        


