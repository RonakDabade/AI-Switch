#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Servo.h>

// Helper files for Firebase
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Firebase Credentials
#define FIREBASE_HOST "aria-52d18-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "7345iF0GCBBg63qXfUuX8MZeIr30RTw2ZCVi4hFC"

// WiFi Credentials
#define WIFI_SSID "Bachelor@VIT"
#define WIFI_PASSWORD "RDGTRT@3"

// Servo Configuration
#define SERVO_PIN D4
#define ANGLE_ON 0    // Servo angle for "ON" state
#define ANGLE_OFF 120 // Servo angle for "OFF" state
Servo myServo;

// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// State Tracking
String previousResult = ""; // Stores last Firebase value

void setup() {
  Serial.begin(115200);

  // Initialize Servo
  myServo.attach(SERVO_PIN);
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure Firebase
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initial sync with Firebase value
  if (Firebase.RTDB.getString(&fbdo, "/status/result")) {
    if (fbdo.dataType() == "string") {
      previousResult = fbdo.stringData();
      if (previousResult == "ON") {
        myServo.write(ANGLE_ON);
        Serial.println("Initialized to ON (0°)");
      } else if (previousResult == "OFF") {
        myServo.write(ANGLE_OFF);
        Serial.println("Initialized to OFF (120°)");
      }
    }
  } else {
    Serial.println("Initial Firebase read failed!");
  }
}

void loop() {
  if (Firebase.RTDB.getString(&fbdo, "/status/result")) {
    if (fbdo.dataType() == "string") {
      String currentResult = fbdo.stringData();

     
      if (currentResult != previousResult) {
        Serial.print("New Firebase Result: ");
        Serial.println(currentResult);

        if (currentResult == "ON") {
          myServo.write(ANGLE_ON);
          Serial.println("Servo turned ON (0°)");
        } else if (currentResult == "OFF") {
          myServo.write(ANGLE_OFF);
          Serial.println("Servo turned OFF (120°)");
        }

        previousResult = currentResult; // Update state
      }
    }
  } else {
    Serial.print("Firebase read failed: ");
    Serial.println(fbdo.errorReason());
  }

  delay(1000); 
}
