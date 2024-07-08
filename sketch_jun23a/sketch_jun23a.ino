#include <SoftwareSerial.h>
#include <EEPROM.h>

// Define the pins for the A9G module
const int RXPin = 4;  // A9G TX pin connected to Arduino RX pin
const int TXPin = 3;  // A9G RX pin connected to Arduino TX pin

// Create a SoftwareSerial object to communicate with the A9G module
SoftwareSerial A9GSerial(RXPin, TXPin);

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  Serial.println("Serial started.");

  // Initialize software serial communication with the A9G module
  A9GSerial.begin(115200);
  Serial.println("SoftwareSerial initialized.");

  // Give the module some time to start up
  delay(2000);

  // Turn on the GPS
  A9GSerial.println("AT+GPS=1");
  delay(1000);

  // Retrieve and print last known location from EEPROM
  float lastLatitude = readFloatFromEEPROM(0);
  float lastLongitude = readFloatFromEEPROM(4);
  Serial.print("Last known Latitude: ");
  Serial.println(lastLatitude, 6);
  Serial.print("Last known Longitude: ");
  Serial.println(lastLongitude, 6);
}

void loop() {
  // Send the AT+LOCATION=2 command to get GPS location
  Serial.println("Sending command: AT+LOCATION=2");
  A9GSerial.println("AT+LOCATION=2");
  
  // Allow time for the command to be processed
  delay(2000); 

  // Read the response from the A9G module with a longer timeout
  unsigned long startTime = millis();
  String response = "";
  while (millis() - startTime < 20000) { // 20 seconds timeout
    while (A9GSerial.available()) {
      char c = A9GSerial.read();
      response += c;
    }
    if (response.length() > 0) {
      Serial.println("Response: " + response);
      break;
    }
  }

  // Check if a valid response was received
  if (response.indexOf("OK") != -1) {
    // Extract latitude and longitude from the response
    float latitude = parseLatitude(response);
    float longitude = parseLongitude(response);

    // Print the latitude and longitude
    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);
    Serial.println();

    // Store the new location in EEPROM
    writeFloatToEEPROM(0, latitude);
    writeFloatToEEPROM(4, longitude);
  } else {
    Serial.println("Failed to get a valid GPS location.");
  }

  // Wait before sending the next command
  delay(5000); // Adjust delay as needed
}

float parseLatitude(String response) {
  int start = response.indexOf(':') + 2;
  int commaIndex = response.indexOf(',', start);
  String latString = response.substring(start, commaIndex);
  return latString.toFloat();
}

float parseLongitude(String response) {
  int start = response.indexOf(':') + 2;
  int firstComma = response.indexOf(',', start);
  int secondComma = response.indexOf(',', firstComma + 1);
  String lonString = response.substring(firstComma + 1, secondComma);
  return lonString.toFloat();
}

void writeFloatToEEPROM(int address, float value) {
  byte* bytePointer = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(float); i++) {
    EEPROM.write(address + i, bytePointer[i]);
  }
}

float readFloatFromEEPROM(int address) {
  float value = 0.0;
  byte* bytePointer = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(float); i++) {
    bytePointer[i] = EEPROM.read(address + i);
  }
  return value;
}
