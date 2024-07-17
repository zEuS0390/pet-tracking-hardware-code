#include <SoftwareSerial.h>

// Define the general baud rate for serial communication
const unsigned long BAUD_RATE_GENERAL = 115200;

// Define the baud rate for the A9G GPS module
const unsigned long BAUD_RATE_A9G_MODULE = 115200;

// Define the RX pin for receiving data from the A9G module
const int A9G_MODULE_RX = 4;

// Define the TX pin for transmitting data to the A9G module
const int A9G_MODULE_TX = 3;

// Define index positions for the GNGGA sentence tokens
const int TIME_INDEX = 1;          // UTC time (HHMMSS.SSS)
const int LAT_INDEX = 2;           // Latitude in DDDMM.MMMM format
const int LAT_DIR_INDEX = 3;       // Latitude direction (N/S)
const int LON_INDEX = 4;           // Longitude in DDDMM.MMMM format
const int LON_DIR_INDEX = 5;       // Longitude direction (E/W)

// Initialize software serial communication for the A9G module
SoftwareSerial A9G(
  A9G_MODULE_RX,
  A9G_MODULE_TX
);

// Buffer to store incoming GPS data (max size 256 characters)
char buffer[256];

// Index to track the current position in the buffer for incoming data
int index = 0;

void setup() {
  // Initialize serial communication at the defined general baud rate
  Serial.begin(BAUD_RATE_GENERAL);
  
  // Initialize the A9G GPS module at the defined baud rate
  A9G.begin(BAUD_RATE_A9G_MODULE);

  // Activate GPS
  A9G.println("AT+GPS=1\r");
  delay(1000);

  // Enable GPS data reading with 5 seconds interval
  A9G.println("AT+GPSRD=5\r");
  delay(1000);
}

void loop() {
  // Check if data is available from the Serial interface
  if (Serial.available()) {
    // Read input until newline and send it to the A9G module
    String input = Serial.readStringUntil('\n');
    A9G.println(input);
  }

  // Process any available data from the A9G GPS module
  while (A9G.available()) {
    // Read a single character from the A9G module
    char c = A9G.read();

    // Ensure we don't exceed the buffer size
    if (index < sizeof(buffer) - 1)
      buffer[index++] = c; // Store character in buffer
    else
      index = 0;

    // Check for the end of a line
    if (c == '\n') {
      // Null-terminate the string
      buffer[index] = '\0';

      // Parse the GPS data from the buffer
      parseGPSData(buffer);

      // Reset index for the next line
      index = 0;
    }
  }

  delay(100);
}

void parseGPSData(char *data) {
    // Locate the start of the GNGGA sentence in the input data
    char *gngga = strstr(data, "$GNGGA");

    if (gngga != NULL) {
        // Array to hold parsed tokens
        char *tokens[15];

        // Tokenize the GNGGA string
        char *token = strtok(gngga, ",");
        int i = 0;

        // Extract tokens up to a maximum of 15
        while (token != NULL && i < 15) {
            // Store each token
            tokens[i++] = token;

            // Get the next token
            token = strtok(NULL, ",");
        }

        /*
          Index     Token Description
          0         Sentence identifier ($GNGGA)
          1         Time (UTC)
          2         Latitude
          3         Latitude direction (N/S)
          4         Longitude
          5         Longitude direction (E/W)
          6         Fix quality
          7         Number of satellites in view
          8         Horizontal dilution of precision
          9         Altitude
          10        Altitude units (M)
          11        Height of geoid (mean sea level)
          12        Height units (M)
          13        Time since last DGPS update
          14        DGPS station ID
        */

        // Ensure there are enough tokens for valid GPS data
        if (i >= 6) {
            char* time = tokens[TIME_INDEX];                // Extract UTC time
            char* latitude = tokens[LAT_INDEX];             // Extract latitude
            char* latDirection = tokens[LAT_DIR_INDEX];     // Extract latitude direction (N/S)
            char* longitude = tokens[LON_INDEX];            // Extract longitude
            char* longDirection = tokens[LON_DIR_INDEX];    // Extract longitude direction (E/W)

            // Convert latitude from DDDMM.MMMM format to decimal degrees
            float lat = convertGPSCoordinateToDecimal(atof(latitude), latDirection[0]);

            // Convert longitude from DDDMM.MMMM format to decimal degrees
            float lon = convertGPSCoordinateToDecimal(atof(longitude), longDirection[0]);

            // Format time into HH:MM:SS format for readability
            String formattedTime = String(time).substring(0, 2) + ":" + 
                                   String(time).substring(2, 4) + ":" + 
                                   String(time).substring(4, 6);

            // Print the formatted UTC time to the Serial Monitor
            Serial.print("Time (UTC): ");
            Serial.println(formattedTime);        

            // Print the parsed latitude to the Serial Monitor
            Serial.print("Latitude: ");
            Serial.println(lat, 6);

            // Print the parsed longitude to the Serial Monitor
            Serial.print("Longitude: ");
            Serial.println(lon, 6);
        }
    }
}

float convertGPSCoordinateToDecimal(float gpsCoordinate, char direction) {
    // Separate degrees and minutes
    int degrees = (int)(gpsCoordinate / 100);
    float minutes = gpsCoordinate - (degrees * 100);
    
    // Convert to decimal
    float decimal = degrees + (minutes / 60);
    
    // Adjust for direction
    if (direction == 'S' || direction == 'W')
        decimal = -decimal;
    
    return decimal;
}