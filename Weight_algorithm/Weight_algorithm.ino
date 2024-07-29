#include <ESP32Servo.h>
#include "HX711.h"
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
const char* password = "";
unsigned long myChannelNumber = 2606703;
const char* myWriteAPIKey = "MEAYCUGPIIB31FF4";

WiFiClient client;

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

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {40, 39, 38, 37};
byte colPins[COLS] = {36, 35, 34, 33};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int ledMode = 0;
float weightThreshold1 = 1.0;
float weightThreshold2 = 2.5;

int firstDoorOpenCount = 0;
int secondDoorOpenCount = 0;
float previousWeight = 0.0;

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

  ThingSpeak.begin(client);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  servoMotor.attach(SERVO_PIN);
  secondServoMotor.attach(SECOND_SERVO_PIN);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up resistor

  Wire.begin(8, 9);
}
