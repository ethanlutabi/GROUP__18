#include <ESP32Servo.h>
#include <HX711.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "ThingSpeak.h"


#define SERVO_PIN 4
#define SECOND_SERVO_PIN 2
#define RED_LED_PIN 13
#define GREEN_LED_PIN 12
#define LOADCELL_DOUT_PIN 18
#define LOADCELL_SCK_PIN 19
#define BUTTON_PIN 5
const char* ssid = "Wokwi-GUEST";
const char* password="";
unsigned long myChannelNumber = 2606703;
const char* myWriteAPIKey = "MEAYCUGPIIB31FF4";

WiFiClient  client;

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
float previousWeight = 0.0; // Store previous weight to detect new load
bool lastSteadyPressureCounted = false; // Flag to track last steady pressure

void setLED(int mode = -1) {
    ledMode = mode;
  }

  switch (ledMode) {
    case 0: 
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
      break;
    case 1: 
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
      break;
    case 2: 
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, HIGH);
      break;
  }
}

void setup() {
  Serial.begin(115200); 
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
  pinMode(BUTTON_PIN, INPUT_PULLUP); 

  Wire.begin(8, 9);
  lcd.begin(16, 2); 
  lcd.backlight();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();

  lcd.print("Initializing...");
  Serial.println("Setup complete");
}


void loop() {
  
  if (key){
    
    if (settingWeight) {
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


  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);

  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");
}
ThingSpeak.begin(client);


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

void setLED(int mode = -1) {
// 0 = none, 1 = red-glow, 2 = green-glow, 3 = blue-glow, 4 = white-glow

  (mode >= 0 && mode <= 4) ? ledMode = mode : ledMode = ledMode;

   switch(ledMode) {
    case 0:
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
    break;
    case 1:
      digitalWrite(R, HIGH);
      digitalWrite(G, LOW);
      digitalWrite(B, LOW);
    break;
    case 2:
      digitalWrite(R, LOW);
      digitalWrite(G, HIGH);
      digitalWrite(B, LOW);
    break;
    case 3:
      digitalWrite(R, LOW);
      digitalWrite(G, LOW);
      digitalWrite(B, HIGH);
    break;
    case 4:
      digitalWrite(R, HIGH);
      digitalWrite(G, HIGH);
      digitalWrite(B, HIGH);
    break;
  }
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

  if (inMenu && (millis() - menuStartTime >= 20000)) {
    lcd.clear();
    inMenu = false;
    settingWeight = false;
    settingMode =0;
  }
bool buttonState = digitalRead(BUTTON_PIN);
if (buttonState == LOW && lastButtonState == HIGH) {
  systemEnabled = !systemEnabled; // Toggle system state
  Serial.print("System ");
  Serial.println(systemEnabled ? "Enabled" : "Disabled");
  delay(500); // Debounce delay
}
lastButtonState = buttonState;
if (systemEnabled && !inMenu) {
  if (scale.is_ready()) {
    long reading = scale.get_units(10);
    float weight = reading / 419.8;
    Serial.print("Weight: ");
    Serial.print(weight, 2);//print the weight with 2 decimal places 
    Serial.println(" kg");
    // lcd.clear();
    lcd.setCursor(0, 0);
