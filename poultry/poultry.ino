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
