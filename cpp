#include <SoftwareSerial.h>

SoftwareSerial gsm(7, 8); // RX, TX for GSM module

#define SOIL_SENSOR A0
#define RELAY 9

String incomingData = "";
int moistureValue = 0;
int dryThreshold = 500; // Adjust based on sensor

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH); // Pump OFF initially

  delay(1000);
  Serial.println("GSM Agriculture System Ready");
  sendSMS("System Started. Monitoring soil moisture.");
}

void loop() {
  // Read soil moisture
  moistureValue = analogRead(SOIL_SENSOR);
  Serial.print("Soil Moisture: ");
  Serial.println(moistureValue);

  // Auto alert if soil is dry
  if (moistureValue > dryThreshold) {
    sendSMS("Soil is dry! Please water the crops.");
    delay(60000); // Wait 1 minute before sending next alert
  }

  // Check for incoming SMS
  if (gsm.available()) {
    char c = gsm.read();
    incomingData += c;

    // End of SMS
    if (c == '\n') {
      Serial.println("Received: " + incomingData);

      if (incomingData.indexOf("PUMP ON") >= 0) {
        digitalWrite(RELAY, LOW); // Turn pump ON
        sendSMS("Pump turned ON");
      }
      else if (incomingData.indexOf("PUMP OFF") >= 0) {
        digitalWrite(RELAY, HIGH); // Turn pump OFF
        sendSMS("Pump turned OFF");
      }

      incomingData = "";
    }
  }
}

// Function to send SMS
void sendSMS(String message) {
  gsm.println("AT+CMGF=1"); // Set text mode
  delay(500);
  gsm.println("AT+CMGS=\"+91xxxxxxxxxx\""); // Change to your phone number
  delay(500);
  gsm.println(message);
  delay(500);
  gsm.write(26); // End of message
  delay(5000);
}
