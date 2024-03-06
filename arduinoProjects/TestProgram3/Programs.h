#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <FastLED.h>

#define NUM_LEDS    256 // Define NUM_LEDS here

#define COOLING     55
#define SPARKING    120

#define LED_PIN     7
#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS    256
#define BRIGHTNESS  64

extern CRGB leds[NUM_LEDS]; // Declare the leds array globally
extern const bool kMatrixSerpentineLayout;
extern const bool kMatrixVertical;
extern Adafruit_NeoMatrix matrix;
extern int scrollSpeed;


void runFireEffect();
void runGradientEffect();
void runScrollText();
void drawGradient(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
uint16_t XY(uint8_t x, uint8_t y);

#endif

