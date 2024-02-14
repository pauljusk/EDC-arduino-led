#include <Serial.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 7
#define LED_WIDTH 8
#define LED_HEIGHT 8

Adafruit_NeoPixel leds(256, LED_PIN, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel leds(LED_WIDTH * LED_HEIGHT, LED_PIN, NEO_GRB + NEO_KHZ800);

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

void setup() {
  Serial.begin(9600);
  leds.begin();
  leds.show();  // Initialize all LEDs to off
}

void loop() {
  displayColorfulHeart();
  delay(100);  // Delay between color changes
}

void displayColorfulHeart() {
  static int x = 0;
// Clear the matrix
  leds.clear();
  x+=LED_HEIGHT;
  if(x>=72){
    x=0;
  } 
  for (uint8_t row = 0; row < LED_HEIGHT; row++) {
    for (uint8_t col = 0; col < LED_WIDTH; col++) {
      if (bitRead(heartPattern[row], col)) {
        uint16_t pixelIndex = row * LED_WIDTH + col +x;
        uint32_t color = getColorFromPalette(pixelIndex);
        Serial.println(color);
        leds.setPixelColor(pixelIndex, color);
        leds.setPixelColor(pixelIndex +72, color);
        leds.setPixelColor(pixelIndex +72*2, color);
        leds.setPixelColor(pixelIndex +72*3, color);
        leds.setPixelColor(pixelIndex +72*4, color);
        leds.setPixelColor(pixelIndex -72*2, color);
        leds.setPixelColor(pixelIndex -72*3, color);
        leds.setPixelColor(pixelIndex -72*4, color);
        leds.setPixelColor(pixelIndex -72, color);
      }
    }
  }

  leds.show();  // Update the LED panel
}

uint32_t getColorFromPalette(uint8_t index) {
  static const uint8_t palette[][3] = {
    {255, 0, 0},     // Red
    {255, 165, 0},   // Orange
    {255, 255, 0},   // Yellow
    {0, 255, 0},     // Green
    {0, 0, 255},     // Blue
    {75, 0, 130},    // Indigo
    {238, 130, 238}  // Violet
  };

  uint8_t paletteSize = sizeof(palette) / sizeof(palette[0]);
  uint8_t colorIndex = index % paletteSize;

  // return leds.Color(palette[colorIndex][0], palette[colorIndex][1], palette[colorIndex][2]);
  return leds.Color(random(0, 256),random(0, 256),random(0, 256));
}
