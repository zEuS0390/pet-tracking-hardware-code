#include "constants.h"
#include "indicator.h"

// Array to hold the RGB values, initialized to 0
unsigned char colors[NUM_COLORS] = {0, 0, 0};

// Index variable to iterate through the colors array
unsigned char i = 0;

void setup() 
{
  // Initialize RGB LED pins
  RGBLED::initialize();
}

void loop() 
{
  // Increment the current color component if it is less than 255
  if (colors[i] < 255) 
  {
    colors[i]++;
  } 
  // If the current color component reaches 255, reset it to 0 and move to the next component
  else 
  {
    colors[i] = 0;
    i = (i + 1) % NUM_COLORS; // Move to the next color component if it does not exceed the number of colors, otherwise reset back to zero
  }

  // Update the RGB LED with the current colors
  RGBLED::setColor(colors[0], colors[1], colors[2]);

  // Wait for the specified delay time in milliseconds
  delay(DELAY_TIME);
}
