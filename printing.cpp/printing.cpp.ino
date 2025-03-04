void setup() {
    Serial.begin(115200);  // Start Serial communication at 115200 baud
    delay(1000);  // Small delay to ensure Serial starts properly
    Serial.println("NodeMCU Initialized!");
}

void loop() {
    Serial.println("Hello from NodeMCU!"); // Print message every second
    delay(1000);
}
