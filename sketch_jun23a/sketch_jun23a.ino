#include <SoftwareSerial.h>

#define ARDUINO_NANO_BAUDRATE     9600
#define A9G_BOARD_BAUDRATE        9600
#define A9G_BOARD_RX              3
#define A9G_BOARD_TX              4

// Initialize software serial communication for the A9G module
SoftwareSerial A9G(
  A9G_BOARD_RX, 
  A9G_BOARD_TX
);

void setup() {
  Serial.begin(ARDUINO_NANO_BAUDRATE);
  A9G.begin(A9G_BOARD_BAUDRATE);

  Serial.println("Starting...");

  A9G.println("AT\r");
  delay(10);
  delay(1000);
  if (!checkResponse("OK")) {
    Serial.println("A9G did not respond.");
    return;
  }

  // Check network registration
  A9G.println("AT+CREG?\r");
  delay(1000);
  if (!checkResponse("1,1") && !checkResponse("1,5")) {
      Serial.println("Module not registered on the network.");
      return;
  }

  activateGPRS();
  if (!checkResponse("OK")) {
    Serial.println("A9G failed to connect to the internet.");
    return;
  }
  Serial.println("GPRS activated successfully.");

  checkSignalQuality();

  checkNetworkRegistration();
}

void loop() {
}

bool checkResponse(const char* response) {
  long int timeout = millis() + 10000;
  while (millis() < timeout) {
    if (A9G.available()) {
      String reply = A9G.readStringUntil('\n');
      if (reply.indexOf(response) != -1) {
        return true;
      }
    }
  }
  return false;
}

void activateGPRS() {
  A9G.println("AT+CGATT=1\r");
  delay(6000);
  
  // Check if GPRS is attached
  if (!checkResponse("OK")) {
    Serial.println("Failed to attach to GPRS.");
    return;
  }

  A9G.println("AT+CGDCONT=1,\"IP\",\"smartlte\"\r");
  delay(1000);
  
  // Optionally, check the response for CGDCONT
  if (!checkResponse("OK")) {
    Serial.println("Failed to set PDP context.");
    return;
  }
}

void checkNetworkRegistration() {
  // Clear any previous responses in the buffer
  while (A9G.available()) {
    A9G.read();
  }

  A9G.println("AT+CREG?\r");
  delay(2000);  // Wait for the response to come in

  String response = "";
  String line = "";
  while (A9G.available()) {
    char c = A9G.read();
    if (c == '\n') {  // End of a line
      line.trim();
      if (line.startsWith("+CREG")) {
        response = line;
      }
      line = "";  // Clear the line buffer for the next line
    } else {
      line += c;
    }
  }

  Serial.println("Response: " + response);

  if (response.indexOf("+CREG: 1,1") != -1 || response.indexOf("+CREG: 1,5") != -1) {
    Serial.println("Module is registered on the network.");
    checkIPAddress();
  } else if (response.indexOf("+CREG: 1,0") != -1 || response.indexOf("+CREG: 1,2") != -1 || response.indexOf("+CREG: 1,3") != -1 || response.indexOf("+CREG: 1,4") != -1) {
    Serial.println("Module is not registered on the network.");
  } else {
    Serial.println("Failed to get network registration status. Response: " + response);
  }
}

void checkIPAddress() {
  // Clear any previous responses in the buffer
  while (A9G.available()) {
    A9G.read();
  }

  A9G.println("AT+CIFSR\r");
  delay(2000);  // Wait for the response to come in

  String response = "";
  String line = "";
  unsigned long startTime = millis();
  
  while (millis() - startTime < 5000) {  // Timeout after 5 seconds
    while (A9G.available()) {
      char c = A9G.read();
      if (c == '\n' || c == '\r') {  // End of a line
        line.trim();
        if (line.length() > 0 && line.indexOf('.') != -1) {
          response = line;
          break;  // Exit the loop once the IP address is found
        }
        line = "";  // Clear the line buffer for the next line
      } else {
        line += c;
      }
    }
  }

  Serial.println("Response: " + response);

  if (response.indexOf('.') != -1) {  // Check if the response contains a dot (.) which is typical in IP addresses
    Serial.println("IP Address obtained successfully.");
    Serial.println("IP Address: " + response);
  } else {
    Serial.println("Failed to obtain IP address.");
  }
}

void checkSignalQuality() {
  /*
  Signal Strength     Example Response
  Excellent (31)      +CSQ: 31,99
  Good (26)           +CSQ: 26,99
  Moderate (20)       +CSQ: 20,99
  Weak (15)           +CSQ: 15,99
  Poor (5)            +CSQ: 5,99
  No Signal (0)       +CSQ: 0,99
  */
  A9G.println("AT+CSQ\r");
  delay(2000);

  String response = "";
  while (A9G.available()) {
    char c = A9G.read();
    response += c;
  }

  Serial.println("Signal Quality Response: " + response);
}
