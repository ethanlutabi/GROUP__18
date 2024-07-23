#include <ESP32Servo.h>
#include <HX711.h>
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
LiquidCrystal_I2C lcd(0x27, 16, 2);

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



const int LOADCELL_DOUT_PIN = 2; // Data output pin
const int LOADCELL_SCK_PIN = 3;  // Clock pin

HX711 scale;

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // Initialize the scale with the specified pins
  Serial.println("Tare - Remove weights"); // Prompt to remove weights
  delay(5000); // Wait for 5 seconds
  scale.tare(); // Reset the scale to 0
  Serial.println("Tare done...");
  Serial.println("Now put weight on the scale"); // Prompt to put weights
  delay(5000); // Wait for 5 seconds
}

void loop() {
  if (scale.is_ready()) {
    long reading = scale.get_units(10); // Get average of 10 readings to calculate weight
    float weight = reading / 419.8; // Divide by calibration factor to get the weight in desired units
    Serial.print("Weight: ");
    Serial.print(weight, 2); // Print the weight with 2 decimal places
    Serial.println(" kg");
  } else {
    Serial.println("HX711 not found.");
  }
  delay(1000); // Delay before the next reading
}
