#include "constants.h"
#include "indicator.h"

void setup() {
  // Initialize RGB LED pins
  RGBLED::initialize();
}

void loop() {
  // Set color to red
  RGBLED::setColor(255, 0, 0);

  // Wait for 1 second
  delay(1000);

  // Set color to green
  RGBLED::setColor(0, 255, 0);

  // Wait for 1 second
  delay(1000);

  // Set color to blue
  RGBLED::setColor(0, 0, 255);

  // Wait for 1 second
  delay(1000);
}
