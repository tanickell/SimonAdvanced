/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "neopixel.h"
#include "IoTTimer.h"
#include "Button.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Constants
const int RANDPIN = D1; // random seed
const int PIXELCOUNT = 4; // neopixel num
const int BRIGHTNESS = 50; // initial/default brightness

const int REDPIN = D10;
const int BLUPIN = D6;
const int YLWPIN = D5;
const int GRNPIN = D4;

const int COLOR_RED = 0xFF0000;
const int COLOR_BLU = 0x0000FF;
const int COLOR_YLW = 0xFFFF00;
const int COLOR_GRN = 0x00FF00;

const int COLOR_RED_INDEX = 0;
const int COLOR_BLU_INDEX = 1;
const int COLOR_YLW_INDEX = 2;
const int COLOR_GRN_INDEX = 3;

const int COLORS[] = {COLOR_RED, COLOR_BLU, COLOR_YLW, COLOR_GRN};

// Variables
int pixelColor;
int speed;
bool gameOver;

// Objects
std::vector<int> solution;
IoTTimer timer;
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);
Button redButton(REDPIN);
Button bluButton(BLUPIN);
Button ylwButton(YLWPIN);
Button grnButton(GRNPIN);

// Function Declarations
void lightPixel(int pixelNum, int pixelColor, int time);
void playSolution();
int getGuess();
bool checkGuess(int guess, int index);
void resetGame();

// setup
void setup() {

  // set up pins
  pinMode(RANDPIN, INPUT);

  pinMode(REDPIN, INPUT);
  pinMode(BLUPIN, INPUT);
  pinMode(YLWPIN, INPUT);
  pinMode(GRNPIN, INPUT);

  // set up serial monitor
  Serial.begin(9600);
  waitUntil(Serial.isConnected);
  delay(1000);
  Serial.printf("Ready to go!\n\n");

  // set up pixel
  pixelColor = 0;
  pixel.begin();
  pixel.setBrightness(BRIGHTNESS);
  pixel.show();

  // set up random
  randomSeed(analogRead(RANDPIN));

  // initialize variables
  speed = 1000;
  gameOver = false;

  resetGame();
}


// loop
void loop() {

  // first, with each loop, play the solution script
  playSolution();

  // for each value in solution, get user guess and check it
  for (int i = 0; i < solution.size(); i++) {
    int guess = getGuess();
    bool isCorrect = checkGuess(guess, i);
    if (!isCorrect) {
      gameOver = true;
      break;
    }
  }

  // if game over, reset game
  if (gameOver) {
    resetGame();
  }
}


// Function Definitions
void lightPixel(int pixelNum, int pixelCol, int time) {
  pixel.clear();
  pixel.show();
  pixel.setPixelColor(pixelNum, pixelCol);
  pixel.show();
  timer.startTimer(time);
  while (!timer.isTimerReady()) { 
    // do nothing 
  }
  pixel.clear();
}

void playSolution() {
  int color = random(0, PIXELCOUNT);
  solution.push_back(color);
  for (int i = 0; i < solution.size(); i++) {
    lightPixel(color, COLORS[color], speed);
  }
}

int getGuess() {
  int guess = -1;
  while (redButton.isPressed()) {
    pixel.setPixelColor(COLOR_RED_INDEX, COLORS[COLOR_RED_INDEX]);
    pixel.show();
    guess = 0;
  }
  while (bluButton.isPressed()) {
    pixel.setPixelColor(COLOR_BLU_INDEX, COLORS[COLOR_BLU_INDEX]);
    pixel.show();
    guess = 1;
  }
  while (ylwButton.isPressed()) {
    pixel.setPixelColor(COLOR_YLW_INDEX, COLORS[COLOR_YLW_INDEX]);
    pixel.show();
    guess = 2;
  }
  while (grnButton.isPressed()) {
    pixel.setPixelColor(COLOR_GRN_INDEX, COLORS[COLOR_GRN_INDEX]);
    pixel.show();
    guess = 3;
  }
  pixel.clear();

  return guess;
}

bool checkGuess(int guess, int index) {
  return solution[index] == guess;
}

void resetGame() {
  // flash lights five times and reset

  timer.startTimer(1000);
  while (!timer.isTimerReady()) {
    // do nothing
  }
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 4; j++) {
      pixel.setPixelColor(i, COLORS[i]);
    }
    pixel.show();
    timer.startTimer(100);
    while (!timer.isTimerReady()) {
      // do nothing
    }
    pixel.clear();
  }

  // reset game state
  gameOver = false;
  solution.clear();
}
