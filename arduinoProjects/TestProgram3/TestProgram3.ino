#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include "Programs.h"  // Include the header file for your programs

// Button Configuration
const int switchPin = 2;  // Pin for the switch

// Program states
enum ProgramState {
  PROGRAM_FIRE,
  PROGRAM_GRADIENT,
  PROGRAM_SCROLL_TEXT,
  NUM_PROGRAMS
};

ProgramState currentState = PROGRAM_FIRE;
// Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, LED_PIN,
//   NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
//   NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
//   NEO_GRB + NEO_KHZ800);

// int scrollSpeed = 50;
static int x = matrix.width();

void setup() {
  delay(3000);  // Initial delay for stability
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(switchPin, INPUT_PULLUP);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setTextColor(matrix.Color(160, 25, 25));
  matrix.setTextSize(1);
}

void loop() {
  int switchState = digitalRead(switchPin);
  if (switchState == LOW) {
    switchProgram();
    delay(250);  // Debounce delay
  }

  switch (currentState) {
    case PROGRAM_FIRE:
      runFireEffect();
      break;
    case PROGRAM_GRADIENT:
      runGradientEffect();
      break;
    case PROGRAM_SCROLL_TEXT:
      runScrollText();
      break;
  }
}

void switchProgram() {
  currentState = static_cast<ProgramState>((currentState + 1) % NUM_PROGRAMS);
}
