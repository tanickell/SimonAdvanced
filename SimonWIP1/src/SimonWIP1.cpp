/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "IoTClassroom_CNM.h"
#include "neopixel.h"
#include "Encoder.h"
#include "math.h"
#include "WemoObj.h"
#include "graphics.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(MANUAL);


// Constants
const int RANDPIN = D1; // random seed
const int BUZZERPIN = D15; // pins
const int REDPIN = D10; // SIMON buttons
const int BLUPIN = D6;
const int YLWPIN = D5;
const int GRNPIN = D4;
const int ENCODER_PIN_A = D8; // encoder input pullup pins
const int ENCODER_PIN_B = D9;
const int HUE_LIGHT_ON_OFF_BUTTON_PIN = D3;
// const int HUE_LIGHT_CYCLE_BUTTON_PIN  = D12;

const int REDPWRPIN = D11;
const int BLUPWRPIN = D12;
const int YLWPWRPIN = D13;
const int GRNPWRPIN = D14;

const int ENCODER_MIN_POSITION = 0; // encoder position
const int ENCODER_MAX_POSITION = 95;

const int COLOR_RED = 0xFF0000; // colors (hex)
const int COLOR_BLU = 0x0000FF;
const int COLOR_YLW = 0xFFFF00;
const int COLOR_GRN = 0x00FF00;

const int TONE_RED = 220; // tones (A3, C4, E4, A4)
const int TONE_BLU = 261;
const int TONE_YLW = 329;
const int TONE_GRN = 440;

const int RED_INDEX = 0; // indices (R, B, Y, G)
const int BLU_INDEX = 1;
const int YLW_INDEX = 2;
const int GRN_INDEX = 3;

const int COLORS[] = {COLOR_RED, COLOR_BLU, COLOR_YLW, COLOR_GRN};
const int TONES[]  = {TONE_RED, TONE_BLU, TONE_YLW, TONE_GRN};
const unsigned char *FACES[] = {redFace, bluFace, ylwFace, grnFace};
const int BUTTONPWRPINS[] = {REDPWRPIN, BLUPWRPIN, YLWPWRPIN, GRNPWRPIN};

const int BULB = 4; // hue
const int HUE_RAINBOW_SIZE = sizeof(HueRainbow) / sizeof(HueRainbow[0]);
const int PIXELCOUNT = 4; // neopixel num
const int BRIGHTNESS = 50; // initial/default brightness

const int WEMO_ONE = 2;
const int WEMO_TWO = 4;

const int MAX_BRIGHTNESS = 255;
const int MAX_SATURATION = 255;

const int OLED_RESET = -1;
const int INTRO_FLASH_COUNT = 8;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;


// Variables
int pixelColor;
int speed;
bool gameOver;
int lastGuess;
int hueLightColor;
bool hueLightOnOff;
bool wemoOneOnOff;
bool wemoTwoOnOff;
int brightness;
int saturation;
int encoderPosition;
int prevEncoderPosition;
int level;
int myWemo;
bool levels[HUE_RAINBOW_SIZE + 1]; // initialize all values to false
bool firstPlay;


// Objects
std::vector<int> solution;
IoTTimer timer;

Button redButton(REDPIN);
Button bluButton(BLUPIN);
Button ylwButton(YLWPIN);
Button grnButton(GRNPIN);
Button hueLightOnOffButton(HUE_LIGHT_ON_OFF_BUTTON_PIN);
// Button hueLightCycleButton(HUE_LIGHT_CYCLE_BUTTON_PIN);

Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);
Adafruit_SSD1306 display(OLED_RESET);
Encoder myEnc(ENCODER_PIN_A, ENCODER_PIN_B);
WemoObj wemoOne(WEMO_ONE);
WemoObj wemoTwo(WEMO_TWO);


// Function Declarations
void playSolution();
void lightPixel(int pixelNum, int pixelColor, int time);
int getGuess();
bool checkGuess(int guess, int index);
void resetGame();
int encoderPositionToBrightness(int position);
void updateBulb();
void cycleColors();
void cycleColorsReverse();
void displaySplash();
void displayFace(const unsigned char *face);


// setup
void setup() {

  // set up pins
  pinMode(RANDPIN, INPUT);
  pinMode(BUZZERPIN, OUTPUT);
  // pinMode(REDPIN, INPUT);
  // pinMode(BLUPIN, INPUT);
  // pinMode(YLWPIN, INPUT);
  // pinMode(GRNPIN, INPUT);

  pinMode(REDPWRPIN, OUTPUT);
  pinMode(BLUPWRPIN, OUTPUT);
  pinMode(YLWPWRPIN, OUTPUT);
  pinMode(GRNPWRPIN, OUTPUT);

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

  // initialize OLED & display splash
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.display();
  // delay(3000);
  display.clearDisplay();

  // set up WiFi
  WiFi.on();
  WiFi.clearCredentials();
  WiFi.setCredentials("IoTNetwork");
  WiFi.connect();
  while (!WiFi.ready()) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("\n\n");

  // initialize variables
  hueLightOnOff = true;
  wemoOneOnOff = false;
  wemoTwoOnOff = false;
  myEnc.write(ENCODER_MAX_POSITION / 2 * 4);
  encoderPosition = myEnc.read() / 4;
  prevEncoderPosition = encoderPosition;
  brightness = MAX_BRIGHTNESS / 2;
  saturation = MAX_SATURATION;
  myWemo = WEMO_ONE;
  firstPlay = true;

  // play splash screen
  displaySplash();

  // reset the game state so we're ready to play
  resetGame();
}


// loop
void loop() {

  if (hueLightOnOffButton.isPressed()) {
    while (hueLightOnOffButton.isPressed()) {
      // do nothing
    }
    timer.startTimer(50);
    while (!timer.isTimerReady()) {
      // do nothing
    }
    hueLightOnOff = !hueLightOnOff;
    if (hueLightOnOff) {
      Serial.printf("Turning on Hue Light #%i\n", BULB);
      updateBulb();
    }
    else {
      Serial.printf("Turning off Hue Light #%i\n", BULB);
      updateBulb();
    }
  }
  
  // if (hueLightCycleButton.isClicked()) {
  //   cycleColors();
  // }

  encoderPosition = (myEnc.read() / 4);

  // bound input to 0-95
  if (encoderPosition < ENCODER_MIN_POSITION) {
    encoderPosition = ENCODER_MIN_POSITION;
    myEnc.write(encoderPosition * 4);
  }
  if (encoderPosition > ENCODER_MAX_POSITION) {
    encoderPosition = ENCODER_MAX_POSITION;
    myEnc.write(encoderPosition * 4);
  }

  // check to see if position has updated & print
  if (encoderPosition != prevEncoderPosition) {
    prevEncoderPosition = encoderPosition;
    brightness = encoderPositionToBrightness(encoderPosition);
    updateBulb();
    Serial.printf("Encoder Position: #%i, Brightness: %i\n", encoderPosition, brightness);
  }

  if (solution.size() > 2 && solution.size() <= 5) {   // set speed & control lights based on "level"
    speed = 666 / 2;
    level = 2;
    if (!levels[level]) {
      cycleColorsReverse();
      levels[level] = true;
      wemoOne.turnOn();
    }
  }
  if (solution.size() > 5 && solution.size() <= 8) {
    speed = 500 / 2;
    level = 3;
    if (!levels[level]) {
      cycleColorsReverse();
      levels[level] = true;
      wemoTwo.turnOn();
    }  
  }
  if (solution.size() > 8 && solution.size() <= 12) {
    speed = 500 / 2;
    level = 4;
    if (!levels[level]) {
      cycleColorsReverse();
      levels[level] = true;
    }  
  }
  if (solution.size() > 12 && solution.size() <= 16) {
    speed = 333 / 2;
    level = 5;
    if (!levels[level]) {
      cycleColorsReverse();
      levels[level] = true;
    }  
  }
  if (solution.size() > 16 && solution.size() <= 20) {
    speed = 250 / 2;
    level = 6;
    if (!levels[level]) {
      cycleColorsReverse();
      levels[level] = true;
    }  
  }
  if (solution.size() > 20) {
    speed = 200 / 2;
    level = 7;
    if (!levels[level]) {
      cycleColorsReverse();
      levels[level] = true;
    }  
  }

  // first, with each loop, play the solution script
  playSolution();   

  // for each value in solution, get user guess and check it
  Serial.printf("Guess: ");   

  for (int i = 0; i < solution.size(); i++) {
    int guess = getGuess();
    lastGuess = guess;
    bool isCorrect = checkGuess(guess, i);
    if (!isCorrect) {
      gameOver = true;
      break;
    }
  }
  Serial.printf("\n");
  displayFace(ntlFace);
  timer.startTimer(500);
  while (!timer.isTimerReady()) {
    // do nothing
  }

  if (gameOver) {   // if game over, reset game
    resetGame();
    timer.startTimer(1000);
    while (!timer.isTimerReady()) {
      // do nothing
    }
    Serial.printf("You lose! Play again.\n\n");
  }
}


// Functions
void playSolution() {
  int color = random(0, PIXELCOUNT);
  solution.push_back(color);
  Serial.printf("Solution: ");
  for (int i = 0; i < solution.size(); i++) {
    lightPixel(solution[i], COLORS[solution[i]], speed);
    Serial.printf("%d ", solution[i]);
  }
  displayFace(ntlFace);
  Serial.printf("\n");
}

void lightPixel(int pixelNum, int pixelCol, int time) {
  digitalWrite(BUTTONPWRPINS[pixelNum], HIGH);
  pixel.setPixelColor(pixelNum, pixelCol);
  pixel.show();
  tone(BUZZERPIN, TONES[pixelNum]);
  displayFace(FACES[pixelNum]);
  timer.startTimer(time);
  while (!timer.isTimerReady()) { 
    // do nothing 
  }
  digitalWrite(BUTTONPWRPINS[pixelNum], LOW);
  pixel.clear();
  pixel.show();
  // displayFace(ntlFace);
  noTone(BUZZERPIN);
  timer.startTimer(time);
  while (!timer.isTimerReady()) {
    // do nothing
  }
}

int getGuess() {
  int guess = -1;
  while (guess == -1) {
    while (redButton.isPressed()) {
      digitalWrite(BUTTONPWRPINS[0], HIGH);
      pixel.setPixelColor(RED_INDEX, COLORS[RED_INDEX]);
      pixel.show();
      guess = 0;
      tone(BUZZERPIN, TONES[RED_INDEX]);
      displayFace(redFace);
    }
    while (bluButton.isPressed()) {
      digitalWrite(BUTTONPWRPINS[1], HIGH);
      pixel.setPixelColor(BLU_INDEX, COLORS[BLU_INDEX]);
      pixel.show();
      guess = 1;
      tone(BUZZERPIN, TONES[BLU_INDEX]);
      displayFace(bluFace);
    }
    while (ylwButton.isPressed()) {
      digitalWrite(BUTTONPWRPINS[2], HIGH);
      pixel.setPixelColor(YLW_INDEX, COLORS[YLW_INDEX]);
      pixel.show();
      guess = 2;
      tone(BUZZERPIN, TONES[YLW_INDEX]);
      displayFace(ylwFace);
    }
    while (grnButton.isPressed()) {
      digitalWrite(BUTTONPWRPINS[3], HIGH);
      pixel.setPixelColor(GRN_INDEX, COLORS[GRN_INDEX]);
      pixel.show();
      guess = 3;
      tone(BUZZERPIN, TONES[GRN_INDEX]);
      displayFace(grnFace);
    }
  }
  digitalWrite(BUTTONPWRPINS[0], LOW);
  digitalWrite(BUTTONPWRPINS[1], LOW);
  digitalWrite(BUTTONPWRPINS[2], LOW);
  digitalWrite(BUTTONPWRPINS[3], LOW);

  noTone(BUZZERPIN);
  // displayFace(ntlFace);
  Serial.printf("%d ", guess);
  pixel.clear();
  pixel.show();
  timer.startTimer(50);   // add a very short delay to prevent "double press" problem
  while (!timer.isTimerReady()) {
    // do nothing
  }

  return guess;
}

bool checkGuess(int guess, int index) {
  return solution[index] == guess;
}

void resetGame() {   // flash lights five times and reset
  timer.startTimer(1000);
  while (!timer.isTimerReady()) {
    // do nothing
  }
  if (!firstPlay) {
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 4; j++) {
        pixel.setPixelColor(j, COLORS[i]);
      }
      pixel.show();
      tone(BUZZERPIN, TONES[lastGuess]);
      displayFace(loseFacePos);
      timer.startTimer(100);
      while (!timer.isTimerReady()) {
        // do nothing
      }
      noTone(BUZZERPIN);
      displayFace(loseFaceNeg);
      pixel.clear();
    }
    wemoOne.turnOff();
    wemoTwo.turnOff();
  }
  firstPlay = false;

  // reset game state
  gameOver = false;
  speed = 750 / 2;
  lastGuess = -1;
  solution.clear();
  hueLightColor = HUE_RAINBOW_SIZE - 1;
  updateBulb();

  int levelsSize = sizeof(levels) / sizeof(levels[0]);
  for (int i = 0; i < levelsSize; i++) {
    levels[i] = false;
  }
  level = 1;
  levels[level] = true;

  displayFace(ntlFace);
}

int encoderPositionToBrightness(int position) {
  return round((((MAX_BRIGHTNESS - 32) / (ENCODER_MAX_POSITION * 1.0)) * position)) + 32;
}

void updateBulb() {
  setHue(BULB, hueLightOnOff, HueRainbow[hueLightColor % HUE_RAINBOW_SIZE], brightness, saturation);
}

void cycleColors() {
  setHue(BULB, hueLightOnOff, HueRainbow[++hueLightColor % HUE_RAINBOW_SIZE], brightness, saturation);
}

void cycleColorsReverse() {
  setHue(BULB, hueLightOnOff, HueRainbow[--hueLightColor % HUE_RAINBOW_SIZE], brightness, saturation);
}

void displaySplash() {
  for (int i = 0; i < INTRO_FLASH_COUNT; i++) {
    display.clearDisplay();
    display.drawBitmap(0, 0, titlePos, OLED_WIDTH, OLED_HEIGHT, WHITE);
    display.display();
    delay(100);
    display.clearDisplay();
    display.drawBitmap(0, 0, titleNeg, OLED_WIDTH, OLED_HEIGHT, WHITE);
    display.display();
    delay(100);
  }
}

void displayFace(const unsigned char *face) {
  display.clearDisplay();
  display.drawBitmap(0, 0, face, OLED_WIDTH, OLED_HEIGHT, WHITE);
  display.display();
}