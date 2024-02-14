#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN 7

// #define NEO_MATRIX_TOP 0x00         ///< Pixel 0 is at top of matrix
// #define NEO_MATRIX_BOTTOM 0x01      ///< Pixel 0 is at bottom of matrix
// #define NEO_MATRIX_LEFT 0x00        ///< Pixel 0 is at left of matrix
// #define NEO_MATRIX_RIGHT 0x02       ///< Pixel 0 is at right of matrix
// #define NEO_MATRIX_CORNER 0x03      ///< Bitmask for pixel 0 matrix corner
// #define NEO_MATRIX_ROWS 0x00        ///< Matrix is row major (horizontal)
// #define NEO_MATRIX_COLUMNS 0x04     ///< Matrix is column major (vertical)
// #define NEO_MATRIX_AXIS 0x04        ///< Bitmask for row/column layout
// #define NEO_MATRIX_PROGRESSIVE 0x00 ///< Same pixel order across each line
// #define NEO_MATRIX_ZIGZAG 0x08      ///< Pixel order reverses between lines
// #define NEO_MATRIX_SEQUENCE 0x08   

// Create an Adafruit_NeoMatrix object
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32,8 , PIN,
  // NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  // NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

// Scroll speed (lower value = faster scrolling)
int scrollSpeed = 50;

void setup() {
  matrix.begin();
  matrix.setTextWrap(false); // Do not wrap text

  // Set text color (R, G, B)
  matrix.setTextColor(matrix.Color(160, 25, 25));

  // Set text size (1 = 6x8 pixels, 2 = 12x16 pixels, etc.)
  matrix.setTextSize(1);
}

void loop() {
  static int x = matrix.width(); // Initial position offscreen

  // Clear the matrix
  matrix.fillScreen(0x0);
  // matrix.fillScreen(0);

  // Set the text to be displayed
  matrix.setCursor(x, 0);
  matrix.print("***EDC LAS VEGAS***");

  // Move the text position
  x--;
  if (x < -matrix.width()*4) {
    x = matrix.width();
  }

  // Display the matrix
  matrix.show();

  // Delay to control scrolling speed
  delay(scrollSpeed);
}
