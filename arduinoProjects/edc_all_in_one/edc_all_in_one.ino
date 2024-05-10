#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define NUM_LEDS 256
#define COOLING 55
#define SPARKING 120
#define LED_PIN 7
#define COLOR_ORDER GRB
#define CHIPSET WS2812
#define BRIGHTNESS 64
#define VOLTS 5
#define MAX_MA 4000
#define LED_WIDTH 8
#define LED_HEIGHT 8

CRGBArray<NUM_LEDS> leds3;
CRGB leds[NUM_LEDS];
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, LED_PIN,
                                               NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leds2 = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;

const int switchPin = 2;  // Pin for the switch
const bool kMatrixSerpentineLayout = true;
const bool kMatrixVertical = false;

// Program states
enum ProgramState {
  // PROGRAM_NEBULA,
  // PROGRAM_WAVES,
  PROGRAM_FIREWORKS,
  PROGRAM_VORTEX,
  PROGRAM_SCROLL_TEXT,
  PROGRAM_HEARTS,
  PROGRAM_TWINKLES,
  NUM_PROGRAMS
};

ProgramState currentState = PROGRAM_FIREWORKS;
// static int x = matrix.width();

// CRGBPalette16 gCurrentPalette;
// CRGBPalette16 gTargetPalette;


void setup() {
  delay(3000);  // Initial delay for stability
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(switchPin, INPUT_PULLUP);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setTextColor(matrix.Color(160, 25, 25));
  matrix.setTextSize(1);

  leds2.begin();
  leds2.setBrightness(20);

  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);
  // FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds3, NUM_LEDS).setCorrection(TypicalLEDStrip);

  chooseNextColorPalette(gTargetPalette);
}

unsigned long lastSwitchTime = 0;
// const unsigned long switchInterval = 15000; // 15 seconds in milliseconds
unsigned long switchInterval = 5000;  // 15 seconds in milliseconds

void loop() {
  int switchState = digitalRead(switchPin);
  if (switchState == LOW) {
    switchProgram();
    delay(250);  // Debounce delay
  }
  // Check if it's time to switch programs
  unsigned long currentTime = millis();
  if (currentTime - lastSwitchTime >= switchInterval) {
    switchProgram();
    lastSwitchTime = currentTime;  // Update last switch time
  }


  switch (currentState) {
    // case PROGRAM_FIRE:
    //   FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    //   runFireEffect();
    //   break;
    // case PROGRAM_WAVES:
    //   drawWaves();
    //   break;
    // case PROGRAM_NEBULA:
    //   drawNebula();
    //   break;
    case PROGRAM_VORTEX:
      drawVortex();
      break;
    case PROGRAM_SCROLL_TEXT:
      runScrollText();
      break;
    case PROGRAM_FIREWORKS:
      drawFireworks();
      break;
    case PROGRAM_HEARTS:
      displayColorfulHeart();
      break;
    case PROGRAM_TWINKLES:
      FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds3, NUM_LEDS).setCorrection(TypicalLEDStrip);
      runDrawTwinkles();
      break;
  }
}

// void switchProgram() {
//   currentState = static_cast<ProgramState>((currentState + 1) % NUM_PROGRAMS);
// }

void switchProgram() {
  // Pick a random program state, excluding the current state to ensure change
  int nextProgram = random(NUM_PROGRAMS);
  while (nextProgram == currentState) {
    nextProgram = random(NUM_PROGRAMS);
  }
  currentState = static_cast<ProgramState>(nextProgram);
  switchInterval = random(500, 10001);
}


//=======================================================

void drawFireworks() {
  matrix.fillScreen(0);
  // drawFireworks();
  static int px = random(matrix.width());
  static int py = random(matrix.height());
  static int stage = 0;
  static int radius = 0;

  if (stage == 0) {
    matrix.drawPixel(px, py, matrix.Color(255, 255, 255));  // White color for shooting effect
    if (py > 0) py--;
    else stage = 1;
  } else if (stage == 1) {
    for (int r = radius; r >= 0; r--) {
      matrix.drawCircle(px, py, r, matrix.ColorHSV(random(65536), 255, 255));
    }
    if (radius < 8) radius++;  // Increase the radius to simulate explosion
    else {
      stage = 0;
      radius = 0;
      px = random(matrix.width());
      py = random(matrix.height());
    }
  }

  matrix.show();
  delay(30);  // Speed of the animation
}




// =====================================================

// CRGB leds[NUM_LEDS];  // Define the leds array globally
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
    if (false) {  // gReverseDirection (not used in this version)
      pixelnumber = (NUM_LEDS - 1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
  FastLED.show();
  FastLED.delay(1000 / 60);
}
// =====================================================

// Function to increase each color component by 1
uint32_t increaseColorByOne(uint32_t color) {
  uint8_t r = (color >> 16) & 0xFF;  // Extract red component
  uint8_t g = (color >> 8) & 0xFF;   // Extract green component
  uint8_t b = color & 0xFF;          // Extract blue component

  // Increase each component by 1
  if (r < 255) r++;
  if (g < 255) g++;
  if (b < 255) b++;

  // Recombine the components into a color
  return matrix.Color(r, g, b);
}

uint8_t r = 100;  // Extract red component
uint8_t g = 10;   // Extract green component
uint8_t b = 100;  // Extract blue component
int brightness = 25;
int rr = 1;
int gg = 1;
int bb = 1;
int rotation = 0;
int randomTemp = 0;

// Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, LED_PIN,
//                                                NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
//                                                NEO_GRB + NEO_KHZ800);
int scrollSpeed = 50;
static int x = matrix.width();

void runScrollText() {
  String text = "EDC LAS VEGAS LET'S GO GO Go";
  runScrollTextCore(text);
}

void runScrollTextCore(String text) {
  matrix.fillScreen(0x0);
  matrix.setCursor(x, 0);
  matrix.print(text);
  r = (rr == 1) ? ((r < 220) ? r + 5 : rr = -1) : ((r > 50) ? r - 5 : rr = 1);
  g = (gg == 1) ? ((g < 220) ? g + 4 : gg = -1) : ((g > 50) ? g - 2 : gg = 1);
  b = (bb == 1) ? ((b < 220) ? b + 11 : bb = -1) : ((b > 50) ? b - 15 : bb = 1);
  matrix.setBrightness(brightness);
  matrix.setTextColor(matrix.Color(r, g, b));
  x--;
  if (x < -matrix.width() * 4) {
    x = matrix.width();
  }
  matrix.show();
  delay(scrollSpeed);
}

// =====================================================
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


// const bool kMatrixSerpentineLayout = true;
// const bool kMatrixVertical = false;
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
// =====================================================


// Adafruit_NeoPixel leds2(256, LED_PIN, NEO_GRB + NEO_KHZ800);

// Heart pattern
const uint8_t heartPattern[LED_HEIGHT] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};
void displayColorfulHeart() {
  static int x = 0;
  // Clear the matrix
  leds2.clear();
  x += LED_HEIGHT;
  int someNumber = 80;
  if (x >= someNumber) {
    x = 0;
  }
  for (uint8_t row = 0; row < LED_HEIGHT; row++) {
    for (uint8_t col = 0; col < LED_WIDTH; col++) {
      if (bitRead(heartPattern[row], col)) {
        uint16_t pixelIndex = row * LED_WIDTH + col + x;
        uint32_t color = getColorFromPalette(pixelIndex);
        Serial.println(color);
        leds2.setPixelColor(pixelIndex, color);
        leds2.setPixelColor(pixelIndex + someNumber, color);
        leds2.setPixelColor(pixelIndex + someNumber * 2, color);
        leds2.setPixelColor(pixelIndex + someNumber * 3, color);
        leds2.setPixelColor(pixelIndex + someNumber * 4, color);
        leds2.setPixelColor(pixelIndex - someNumber * 2, color);
        leds2.setPixelColor(pixelIndex - someNumber * 3, color);
        leds2.setPixelColor(pixelIndex - someNumber * 4, color);
        leds2.setPixelColor(pixelIndex - someNumber, color);
      }
    }
  }

  leds2.show();  // Update the LED panel
  delay(50);
}


// uint32_t getColorFromPalette(uint8_t index) {
//   static const uint8_t palette[][3] = {
//     {255, 0, 0},     // Red
//     {255, 165, 0},   // Orange
//     {255, 255, 0},   // Yellow
//     {0, 255, 0},     // Green
//     {0, 0, 255},     // Blue
//     {75, 0, 130},    // Indigo
//     {238, 130, 238}  // Violet
//   };

//   uint8_t paletteSize = sizeof(palette) / sizeof(palette[0]);
//   uint8_t colorIndex = index % paletteSize;

//   return leds2.Color(palette[colorIndex][0], palette[colorIndex][1], palette[colorIndex][2]);
// }
uint32_t getColorFromPalette(uint8_t index) {
  static const uint8_t palette[][3] = {
    { 255, 0, 0 },     // Red
    { 255, 165, 0 },   // Orange
    { 255, 255, 0 },   // Yellow
    { 0, 255, 0 },     // Green
    { 0, 0, 255 },     // Blue
    { 75, 0, 130 },    // Indigo
    { 238, 130, 238 }  // Violet
  };

  uint8_t paletteSize = sizeof(palette) / sizeof(palette[0]);
  uint8_t colorIndex = index % paletteSize;

  return leds2.Color(palette[colorIndex][0], palette[colorIndex][1], palette[colorIndex][2]);
  // return leds2.Color(random(0, 256),random(0, 256),random(0, 256));
}


//=====================================================

#include "FastLED.h"


// #define NUM_LEDS      256
// #define LED_TYPE   WS2811
// #define COLOR_ORDER   GRB
// #define DATA_PIN        7
//#define CLK_PIN       4
// #define VOLTS          12
// #define MAX_MA       4000

// CRGBArray<NUM_LEDS> leds;

#define TWINKLE_SPEED 8
#define TWINKLE_DENSITY 4
#define SECONDS_PER_PALETTE 1
CRGB gBackgroundColor = CRGB::Black;
#define AUTO_SELECT_BACKGROUND_COLOR 0
#define COOL_LIKE_INCANDESCENT 1


// CRGBArray<NUM_LEDS> leds3;
// CRGBPalette16 gCurrentPalette;
// CRGBPalette16 gTargetPalette;

void runDrawTwinkles() {
  EVERY_N_SECONDS(SECONDS_PER_PALETTE) {
    chooseNextColorPalette(gTargetPalette);
  }

  EVERY_N_MILLISECONDS(10) {
    nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
  }

  drawTwinkles(leds3);

  FastLED.show();
}
void drawTwinkles(CRGBSet& L) {
  uint16_t PRNG16 = 11337;
  uint32_t clock32 = millis();
  CRGB bg;
  if ((AUTO_SELECT_BACKGROUND_COLOR == 1) && (gCurrentPalette[0] == gCurrentPalette[1])) {
    bg = gCurrentPalette[0];
    uint8_t bglight = bg.getAverageLight();
    if (bglight > 64) {
      bg.nscale8_video(16);  // very bright, so scale to 1/16th
    } else if (bglight > 16) {
      bg.nscale8_video(64);  // not that bright, so scale to 1/4th
    } else {
      bg.nscale8_video(86);  // dim, scale to 1/3rd.
    }
  } else {
    bg = gBackgroundColor;  // just use the explicitly defined background color
  }

  uint8_t backgroundBrightness = bg.getAverageLight();

  for (CRGB& pixel : L) {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;  // next 'random' number
    uint16_t myclockoffset16 = PRNG16;          // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;  // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t myunique8 = PRNG16 >> 8;  // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle(myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if (deltabright >= 32 || (!bg)) {
      // If the new pixel is significantly brighter than the background color,
      // use the new color.
      pixel = c;
    } else if (deltabright > 0) {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      pixel = blend(bg, c, deltabright * 8);
    } else {
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      pixel = bg;
    }
  }
}


CRGB computeOneTwinkle(uint32_t ms, uint8_t salt) {
  uint16_t ticks = ms >> (8 - TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY) {
    bright = attackDecayWave8(fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if (bright > 0) {
    c = ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
    if (COOL_LIKE_INCANDESCENT == 1) {
      coolLikeIncandescent(c, fastcycle8);
    }
  } else {
    c = CRGB::Black;
  }
  return c;
}


// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//

uint8_t attackDecayWave8(uint8_t i) {
  if (i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent(CRGB& c, uint8_t phase) {
  if (phase < 128) return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8(c.g, cooling);
  c.b = qsub8(c.b, cooling * 2);
}

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM = { CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
                                                          CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
                                                          CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
                                                          CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red 0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM = { Holly_Green, Holly_Green, Holly_Green, Holly_Green,
                                                  Holly_Green, Holly_Green, Holly_Green, Holly_Green,
                                                  Holly_Green, Holly_Green, Holly_Green, Holly_Green,
                                                  Holly_Green, Holly_Green, Holly_Green, Holly_Red };

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM = { CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
                                                     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
                                                     CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
                                                     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM = { CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
                                                      CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
                                                      CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
                                                      CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM = { CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight,
                                                       HALFFAIRY, HALFFAIRY, CRGB::FairyLight, CRGB::FairyLight,
                                                       QUARTERFAIRY, QUARTERFAIRY, CRGB::FairyLight, CRGB::FairyLight,
                                                       CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM = { 0x304048, 0x304048, 0x304048, 0x304048,
                                                 0x304048, 0x304048, 0x304048, 0x304048,
                                                 0x304048, 0x304048, 0x304048, 0x304048,
                                                 0x304048, 0x304048, 0x304048, 0xE0F0FF };

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red 0xB80400
#define C9_Orange 0x902C02
#define C9_Green 0x046002
#define C9_Blue 0x070758
#define C9_White 0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM = { C9_Red, C9_Orange, C9_Red, C9_Orange,
                                                    C9_Orange, C9_Red, C9_Orange, C9_Red,
                                                    C9_Green, C9_Green, C9_Green, C9_Green,
                                                    C9_Blue, C9_Blue, C9_Blue,
                                                    C9_White };

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM = {
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3
};


// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16* ActivePaletteList[] = {
  &RetroC9_p,
  &BlueWhite_p,
  &RainbowColors_p,
  &FairyLight_p,
  &RedGreenWhite_p,
  &PartyColors_p,
  &RedWhite_p,
  &Snow_p,
  &Holly_p,
  &Ice_p
};


// Advance to the next color palette in the list (above).
void chooseNextColorPalette(CRGBPalette16& pal) {
  const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
  static uint8_t whichPalette = -1;
  whichPalette = addmod8(whichPalette, 1, numberOfPalettes);

  pal = *(ActivePaletteList[whichPalette]);
}



// ============================

void drawVortex() {
  static float angle = 0;
  angle += 0.303;  // Speed of rotation
  for (int y = 0; y < matrix.height(); y++) {
    for (int x = 0; x < matrix.width(); x++) {
      float dist = sqrt(pow(x - matrix.width() / 2, 2) + pow(y - matrix.height() / 2, 2));
      float hue = (angle * 180 / PI + dist * 10);
      matrix.drawPixel(x, y, matrix.ColorHSV((long)hue % 65536, (long)hue % 255, (long)hue % 6512));
    }
  }
  matrix.show();
  delay(30);  // Speed of the animation
}

// =========================

float wave[32][8];
void drawWaves() {
  matrix.fillScreen(0);
  for (int x = 1; x < matrix.width() - 1; x++) {
    for (int y = 1; y < matrix.height() - 1; y++) {
      wave[x][y] = (wave[x - 1][y] + wave[x + 1][y] + wave[x][y - 1] + wave[x][y + 1]) / 2 - wave[x][y];
      wave[x][y] *= 0.99;  // Damping
    }
  }
  // Random drops
  if (random(10) == 0) {
    int x = random(1, matrix.width() - 1);
    int y = random(1, matrix.height() - 1);
    wave[x][y] = 255.0;
  }


  for (int x = 0; x < matrix.width(); x++) {
    for (int y = 0; y < matrix.height(); y++) {
      int value = abs((int)wave[x][y]);
      matrix.drawPixel(x, y, matrix.Color(0, 0, value));
    }
  }

  matrix.show();
  delay(50);
}

// ==================
float t = 0.0;
void drawNebula() {
  matrix.fillScreen(0);
  t += 0.01;
  for (int x = 0; x < matrix.width(); x++) {
    for (int y = 0; y < matrix.height(); y++) {
      float nx = x * 0.04, ny = y * 0.04, nt = t * 0.5;
      float r = noise(nx, ny, nt);
      float g = noise(nx + 5, ny + 15, nt + 25);
      float b = noise(nx + 25, ny + 5, nt + 40);
      matrix.drawPixel(x, y, matrix.Color(r * 255, g * 255, b * 255));
    }
  }
  matrix.show();
  delay(100);
}

float noise(float x, float y, float z) {
  // This would normally use a Perlin noise library to generate values
  return (sin(x * 10) + cos(y * 10 + z)) / 2 + 0.5;
}
