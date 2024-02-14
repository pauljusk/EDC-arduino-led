//fire2021_____________________________________
//Perlin noise fire procedure
//Yaroslaw Turbin, 22.06.2020
//https://twitter.com/ldir_ko
//https://www.reddit.com/user/ldirko/

//mask modification for u/thunderdarkdevil

// for https://www.reddit.com/r/FastLED/comments/qq2qda/how_to_use_fire2012_on_part_of_an_array/

#include <FastLED.h>

#define DATA_PIN    7                //set your leds datapin   change to 32 for m5 atom lite

#define LED_TYPE    WS2812B          //leds type
#define COLOR_ORDER GRB              //color order of leds

#define MAX_POWER_MILLIAMPS 800      //write here your power in milliamps. default i set 800 mA for safet

#define NUM_COLS 60            // resolution of planar lookup table
#define NUM_ROWS 10            // resolution of planar lookup table
#define NUM_LEDS NUM_COLS*NUM_ROWS

uint16_t lastSafeIndex = NUM_LEDS;

CRGB leds [NUM_LEDS + 1];

byte brigtness = 255;    //bright on start
byte InitNeeded = 1;
uint8_t gCurrentPatternNumber = 0;    // Index number of which pattern is current


#include "tables.h"

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  // FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
  FastLED.setBrightness(brigtness);
  FastLED.clear();
}


uint16_t XY(byte x, byte y) {     // calculate index in leds from XY coordinates for planar mapping
  uint16_t ledsindex = pgm_read_word (PlanarTable + y * NUM_COLS + x);
  return (ledsindex);
}


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns =     // this is list of patterns
{
  pacman, patrice, warzone, pacific
};


void loop() {
  EVERY_N_SECONDS( 20 ) {                          // speed of change patterns periodically
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns); //next effect
    InitNeeded = 1; //flag if init something need
  } 

  gPatterns[gCurrentPatternNumber](); //play current pattern

  // // cap the refresh rate to 60Hz  // thx to satubarosu
  // uint32_t fps_goal_us = 1000000 / 60;
  // static uint64_t next_frame_us = 0;
  // next_frame_us += fps_goal_us;
  // while (uint32_t delay_us = next_frame_us - micros() < fps_goal_us)
  //   if (delay_us >= 8)
  //     delayMicroseconds(delay_us);
}



DEFINE_GRADIENT_PALETTE(firepal) { // define fire palette
  0,  0,  0,  0, //black
  32,  255,  0,  0, // red
  190,  255,  255,  0, //yellow
  255,  255,  255,  255 // white
};

void patrice () {

  FastLED.setBrightness(255); // bright of effect

  if (InitNeeded) {FastLED.clear(); InitNeeded = 0;}

  CRGBPalette16 myPal = firepal;
  int  a = millis();
  int  a1 = a / 3;
  int maskindex = 0;

  for (byte j = 0; j < NUM_ROWS; j++) {
    for (byte i = 0; i < NUM_COLS; i++) {
      int ledsindex = XY( i, j);

      if (pgm_read_byte (Mask + maskindex++))
        leds[ledsindex] = ColorFromPalette(myPal, qsub8 (inoise8 (i * 60, j * 80 + a, a1),
                                           abs8(j - (NUM_ROWS - 1)) * 255 / (NUM_ROWS + 6)), 255);
    }
  }

  FastLED.show();

}



DEFINE_GRADIENT_PALETTE(firepal1) { // define fire palette
  0,  0,  0,  0, //black
  32,  255,  0,  0, // red
  190,  255,  255,  0, //yellow
  255,  255,  255,  255 // white
};

void warzone () {

  FastLED.setBrightness(255); // bright of effect

  if (InitNeeded) {FastLED.clear(); InitNeeded = 0;}

  CRGBPalette16 myPal = firepal1;
  int  a = millis();
  int  a1 = a / 3;
  int maskindex = 0;

  for (byte j = 0; j < NUM_ROWS; j++) {
    for (byte i = 0; i < NUM_COLS; i++) {
      int ledsindex = XY( i, j);

      if (pgm_read_byte (Mask1 + maskindex++))
        leds[ledsindex] = ColorFromPalette(myPal, qsub8 (inoise8 (i * 60, j * 80 + a, a1),
                                           abs8(j - (NUM_ROWS - 1)) * 255 / (NUM_ROWS + 6)), 255);
    }
  }

  FastLED.show();

}



DEFINE_GRADIENT_PALETTE(firepal2) { // define fire palette
  0,  0,  0,  0, //black
  32,  255,  0,  0, // red
  190,  255,  255,  0, //yellow
  255,  255,  255,  255 // white
};

void pacific () {

  FastLED.setBrightness(255);  // bright of effect

  if (InitNeeded) {FastLED.clear(); InitNeeded = 0;}

  CRGBPalette16 myPal = firepal2;
  int  a = millis();
  int  a1 = a / 3;
  int maskindex = 0;

  for (byte j = 0; j < NUM_ROWS; j++) {
    for (byte i = 0; i < NUM_COLS; i++) {
      int ledsindex = XY( i, j);

      if (pgm_read_byte (Mask2 + maskindex++))
        leds[ledsindex] = ColorFromPalette(myPal, qsub8 (inoise8 (i * 60, j * 80 + a, a1),
                                           abs8(j - (NUM_ROWS - 1)) * 255 / (NUM_ROWS + 6)), 255);
    }
  }

  FastLED.show();

}


void pacman() {
  
  FastLED.setBrightness(255);  // bright of effect
  delay (60);       //delay for animation 

  static byte frame = 0;
  static byte path = 0;
  byte next_Sprite_offset = 20;  //offset for next sprite
  byte virtWight = next_Sprite_offset*5;  //how many cols of virtual screen 
  byte xIndex=0;
  int Leds_index;

  FastLED.clear();

  int index = frame*100;
  
  for (int i = 0; i < 3; i++) {  //draw scroll dots 
    for (int x = 0; x < 20; x++) {
      for (int y = 0; y < 2; y++) {
        int index = XY (x+i*20, 4+y);
        leds[index] = pgm_read_dword (Dots+y*20+(x+path)%4);  //for scroll dots
        // leds[index] = pgm_read_dword (Dots+y*20+x);   //for stay dots

      }
    }
  }

  for (int x = 0; x < 10; x++) {   //main draw cycle 
    for (int y = 0; y < 10; y++) {
     
      int Spr_index = (y<<3)+(y<<1)+index+x;

      xIndex= (x+path)%virtWight;  //draw pacman
      if (xIndex<NUM_COLS){
        Leds_index = XY (xIndex, y);
        leds[Leds_index] = pgm_read_dword (Mask11+Spr_index);
      }

      xIndex = (x+path+next_Sprite_offset)%virtWight; //draw 1 ghost
      if (xIndex<NUM_COLS) {
        Leds_index = XY (xIndex, y);
        leds[Leds_index] = pgm_read_dword (Mask42+Spr_index);
      }

      xIndex = (x+path+next_Sprite_offset*2)%virtWight;  //draw 2 ghost
      if (xIndex<NUM_COLS) {
        Leds_index = XY (xIndex, y);
        leds[Leds_index] = pgm_read_dword (Mask41+Spr_index);
      }

      xIndex = (x+path+next_Sprite_offset*3)%virtWight;  //draw 3 ghost
      if (xIndex<NUM_COLS) {
       Leds_index = XY (xIndex, y);
       leds[Leds_index] = pgm_read_dword (Mask43+Spr_index);
      }

      xIndex = (x+path+next_Sprite_offset*4)%virtWight;  //draw 4 ghost
      if (xIndex<NUM_COLS) {
        Leds_index = XY (xIndex, y);
        leds[Leds_index] = pgm_read_dword (Mask44+Spr_index);
      }

    }
  }

  frame=(frame+1)%4;  //next frame of animation
  path=(path+1)%virtWight; //scroll virtual frame by one pixel

  FastLED.show();
  
}