#include <HX711.h>
#include <LiquidCrystal.h>
const int LOADCELL_DOUT_PIN = 2; // Data output pin
const int LOADCELL_SCK_PIN = 3;  // Clock pin
#include <WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* apiKey = "U81MTWD7LB30F0UR"; // Replace with your ThingSpeak API key
const unsigned long channelID = 2597056; // Replace with your ThingSpeak channel ID

WiFiClient client; // Declare the WiFiClient object

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

  int ledMode = 0;
float weightThreshold1 = 1.0; // Default threshold for the first door
float weightThreshold2 = 2.5; // Default threshold for the second door

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
    float weight = reading / 419.8; // Divide by calibration factor to get the weight in desired units
    Serial.print("Weight: ");
    Serial.print(weight, 2); // Print the weight with 2 decimal places
    Serial.println(" kg");
  } else {
    Serial.println("HX711 not found.");
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