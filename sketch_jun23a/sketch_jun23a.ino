#include <SoftwareSerial.h>

SoftwareSerial A9G(3, 4); // RX, TX for A9G module

void setup() {
  Serial.begin(9600);
  A9G.begin(9600);
  
  Serial.println("Starting...");
  A9G.println("\r");
  A9G.println("AT\r");
  delay(10);

  //To connect to the internet
  A9G.println("AT+CGATT=1\r");

  delay(6000);
  // Set the PDP context
  A9G.println("AT+CGDCONT=1,\"IP\",\"smartlte\"\r");
  delay(1000);
  if (checkResponse("OK")) {
    Serial.println("A9G has been connected to the internet");
  } else {
    Serial.println("Failed to connect A9G to the internet");
  }

  delay(7000);
  // Initialize HTTP service
  A9G.println("AT+HTTPINIT\r");
  delay(7000);
  if (!checkResponse("OK")) {
    Serial.println("Failed to initialize HTTP service");
    return;
  }
}

void loop() {
  // Nothing to do in the loop
}

bool checkResponse(const char* response) {
  long int timeout = millis() + 5000;
  while (millis() < timeout) {
    if (A9G.available()) {
      String reply = A9G.readString();
      if (reply.indexOf(response) != -1) {
        return true;
      }
    }
  }
  return false;
}
