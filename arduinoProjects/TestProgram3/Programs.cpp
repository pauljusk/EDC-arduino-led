#include <Adafruit_NeoMatrix.h> // Add this line to include the Adafruit NeoMatrix library
#include "Programs.h"

CRGB leds[NUM_LEDS]; // Define the leds array globally

// Define the missing constants
const bool kMatrixSerpentineLayout = true;
const bool kMatrixVertical = false;
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, LED_PIN,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);
int scrollSpeed = 50;
static int x = matrix.width();

// void runFireEffect() {
//   // Your implementation for runFireEffect
// }

void runFireEffect() {
  static uint8_t heat[NUM_LEDS];
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  if (random8() < SPARKING) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }
  for (int j = 0; j < NUM_LEDS; j++) {
    uint8_t colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(HeatColors_p, colorindex);
    int pixelnumber;
    if (false) { // gReverseDirection (not used in this version)
      pixelnumber = (NUM_LEDS - 1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
  FastLED.show();
  FastLED.delay(1000 / 60);
}

void runGradientEffect() {
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t)cos16(ms * (27 / 1)) * (350 / 16));
  int32_t xHueDelta32 = ((int32_t)cos16(ms * (39 / 1)) * (310 / 16));
  drawGradient(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  if (ms < 5000) {
    FastLED.setBrightness(scale8(BRIGHTNESS, (ms * 256) / 5000));
  } else {
    FastLED.setBrightness(BRIGHTNESS);
  }
  FastLED.show();
}

// void runGradientEffect() {
//   // Your implementation for runGradientEffect
// }

// void runScrollText() {
//   // Your implementation for runScrollText
// }

void runScrollText() {
  matrix.fillScreen(0x0);
  matrix.setCursor(x, 0);
  matrix.print("***EDC LAS VEGAS***");
  x--;
  if (x < -matrix.width() * 4) {
    x = matrix.width();
  }
  matrix.show();
  delay(scrollSpeed);
}


uint16_t XY(uint8_t x, uint8_t y) {
  uint16_t i;
  if (kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * 16) + x;
    } else {
      i = 16 * (16 - (x + 1)) + y;
    }
  }

  if (kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if (y & 0x01) {
        uint8_t reverseX = (16 - 1) - x;
        i = (y * 16) + reverseX;
      } else {
        i = (y * 16) + x;
      }
    } else {
      if (x & 0x01) {
        i = 16 * (16 - (x + 1)) + y;
      } else {
        i = 16 * (16 - x) - (y + 1);
      }
    }
  }
  return i;
}

void drawGradient(uint8_t startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
  uint8_t lineStartHue = startHue8;
  for (uint8_t y = 0; y < 16; y++) {
    lineStartHue += yHueDelta8;
    uint8_t pixelHue = lineStartHue;
    for (uint8_t x = 0; x < 16; x++) {
      pixelHue += xHueDelta8;
      leds[XY(x, y)] = CHSV(pixelHue, 255, 255);
    }
  }
}

