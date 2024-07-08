#ifndef INDICATOR_H
#define INDICATOR_H

#include "constants.h"

namespace RGBLED {

  void initialize() {
    pinMode(LED_RED_PIN,    OUTPUT);
    pinMode(LED_GREEN_PIN,  OUTPUT);
    pinMode(LED_BLUE_PIN,   OUTPUT);
  }

  void setColor(unsigned char red, unsigned char green, unsigned char blue) {
    analogWrite(LED_RED_PIN,    red);
    analogWrite(LED_GREEN_PIN,  green);
    analogWrite(LED_BLUE_PIN,   blue);
  }
  
}

#endif