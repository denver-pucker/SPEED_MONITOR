/* 
 * Project LED_MATRIX_TEST_1
 * Author: Adrian Montoya
 * Date: 12 APRIL 2025
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "neopixel.h"
#include "Colors.h"

// const int SPI1 = D2;
const int MATRIX_WIDTH = 16;
const int MATRIX_HEIGHT = 16;
const int NUM_PIXELS = (MATRIX_WIDTH * MATRIX_HEIGHT);
const int LED_TYPE = WS2812B;

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(NUM_PIXELS, SPI1, LED_TYPE);

// Functions
uint16_t getPixelIndex(uint8_t x, uint8_t y);
void clearMatrix();
void drawDigit(int digit, int xOffset, int yOffset, uint8_t r, uint8_t g, uint8_t b);
void drawNumber2DigitAnimated(int number, int fadeSteps, int delayPerStep);

// 5x7 font for digits 0–9
const byte numbers[10][5] = {
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
  {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
  {0x62, 0x51, 0x49, 0x49, 0x46}, // 2
  {0x22, 0x49, 0x49, 0x49, 0x36}, // 3
  {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
  {0x2F, 0x49, 0x49, 0x49, 0x31}, // 5
  {0x3E, 0x49, 0x49, 0x49, 0x32}, // 6
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
  {0x26, 0x49, 0x49, 0x49, 0x3E}  // 9
};

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

void setup() {
  matrix.begin();
  matrix.show();
}

void loop() {
  for (int i = 0; i <= 99; i++) {
    drawNumber2DigitAnimated(i, 50, 40);  // Define FADE & DELAY
    delay(800); // Hold after fade
  }
}

uint16_t getPixelIndex(uint8_t x, uint8_t y) {
  y = MATRIX_HEIGHT - 1 - y; // Flip Y
  if (y % 2 == 0) {
    return y * MATRIX_WIDTH + x;
  } else {
    return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  }
}

void clearMatrix() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    matrix.setPixelColor(i, 0);
  }
  matrix.show();
}

// Draw a digit at (xOffset, yOffset) with specified brightness
void drawDigit(int digit, int xOffset, int yOffset, uint8_t r, uint8_t g, uint8_t b) {
  if (digit < 0 || digit > 9) return;
  for (int col = 0; col < 5; col++) {
    byte column = numbers[digit][col];
    for (int row = 0; row < 7; row++) {
      if (column & (1 << row)) {
        int x = xOffset + col;
        int y = yOffset + row;
        if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
          matrix.setPixelColor(getPixelIndex(x, y), r, g, b);
        }
      }
    }
  }
}

// Centered 2-digit draw with fade-in animation
void drawNumber2DigitAnimated(int number, int fadeSteps, int delayPerStep) {
  if (number < 0) number = 0;
  if (number > 99) number = 99;

  int tens = number / 10;
  int ones = number % 10;

  // Total width of 2 digits = 5 + 1 + 5 = 11
  int totalWidth = 11;
  int xOffset = (MATRIX_WIDTH - totalWidth) / 2;
  int yOffset = (MATRIX_HEIGHT - 7) / 2;

  // Fade in
  for (int i = 1; i <= fadeSteps; i++) {
    clearMatrix();
    uint8_t r = (i * 0x00) / fadeSteps;
    uint8_t g = (i * 0xFF) / fadeSteps;  // Greenish fade-in
    uint8_t b = (i * 0xFF) / fadeSteps;

    drawDigit(tens, xOffset, yOffset, r, g, b);
    drawDigit(ones, xOffset + 6, yOffset, r, g, b);
    matrix.show();
    delay(delayPerStep);
  }
}