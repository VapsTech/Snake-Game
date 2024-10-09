// C++ Code
// Snake Game

#include <LedControl.h> // Library for LED Matrix 8x8 control

bool start = true;

const int xAxis = A1;
const int yAxis = A2;

float xValue;
float yValue;

const int Button = 3;
int stateButton;

const int Buzzer = 7;
const int hertzWin = 4000;
const int hertzLose = 2000;

int headRow = 4;
int headCol = 4;

byte bodyRow[63]; // Array to store the positions of body segments
byte bodyCol[63];
int bodyLength = 0;

int fruitRow;
int fruitCol;
bool fruitCheck;

const int speed = 190;

int direction = 0; 

LedControl lc = LedControl(12, 10, 11, 1);
/*
 pin 12 is connected to the DataIn
 pin 11 is connected to the CLK
 pin 10 is connected to LOAD
 Only 1 single MAX72XX.
 */

void setup() {

  lc.shutdown(0, false); // Wake Up call for display
  lc.setIntensity(0, 8); // Set Brightness (0 - 15)
  lc.clearDisplay(0); // Clear Display

  pinMode(Button, INPUT_PULLUP);
  pinMode(Buzzer, OUTPUT);

  randomSeed(A0);

  attraction();

}

void loop() {

  moveSnake();
  updateBody();
  joystick();

}

void attraction() {

  while (start) {

    for(int j = 0; j < 8; j++){
      for(int i = 7; i >= 0; i--){
        lc.setLed(0, j, i, true);
        delay(50);
        
        stateButton = digitalRead(Button);

        if (stateButton == LOW){
          start = false;
        }
        
      }
    }

    lc.clearDisplay(0);

    stateButton = digitalRead(Button);
    if (stateButton == LOW) {
      start = false;
    }
  }

  generateFood();

}

void generateFood() {

  lc.setLed(0, fruitRow, fruitCol, false);

  bodyRow[bodyLength] = bodyRow[bodyLength - 1];
  bodyCol[bodyLength] = bodyCol[bodyLength - 1];
  bodyLength++;

  fruitCheck = false; 

  while (!fruitCheck){ 

    fruitRow = random(8);
    fruitCol = random(8);

    fruitCheck = true;

    for (int i = 0; i <= bodyLength; i++){ 
      if (fruitRow == bodyRow[i] && fruitCol == bodyCol[i]){
        fruitCheck = false;
        break;
      }
    }

  }

  lc.setLed(0, fruitRow, fruitCol, true);

}

void moveSnake() {

  lc.clearDisplay(0);

  // Move the snake automatically based on the current direction
  switch (direction) {
    case 3: // Left
      headRow--;
      break;
    case 0: // Right
      headCol++;
      break;
    case 1: // Up
      headRow++;
      break;
    case 2: // Down
      headCol--;
      break;
  }

  // Wrap the snake around the edges of the LED matrix
  if (headCol > 7) {
    headCol = 0;
  }
  if (headCol < 0) {
    headCol = 7;
  }
  if (headRow > 7) {
    headRow = 0;
  }
  if (headRow < 0) {
    headRow = 7;
  }

  lc.setLed(0, fruitRow, fruitCol, true);
  lc.setLed(0, headRow, headCol, true);

  for (int i = 0; i < bodyLength; i++) {
    lc.setLed(0, bodyRow[i], bodyCol[i], true);
  }
  delay(speed);

  if (checkPoint()) {
    generateFood();
  }

  if (checkCollision()) {
    gameOver();
  }
}

void updateBody() {

  for (int j = bodyLength; j > 0; j--) {
    bodyRow[j] = bodyRow[j - 1];
    bodyCol[j] = bodyCol[j - 1];
  }
  bodyRow[0] = headRow;
  bodyCol[0] = headCol;

}

void joystick() {
  xValue = analogRead(xAxis);
  yValue = analogRead(yAxis);
  
  // Flag to track if a direction change has already occurred
  bool directionChanged = false;

  // Change snake's direction based on joystick input
  if (xValue > 600 && direction != 3 && !directionChanged) {
    direction = 1; // Left
    directionChanged = true;
  }
  if (xValue < 400 && direction != 1 && !directionChanged) {
    direction = 3; // Right
    directionChanged = true;
  }
  if (yValue > 600 && direction != 0 && !directionChanged) {
    direction = 2; // Down
    directionChanged = true;
  }
  if (yValue < 400 && direction != 2 && !directionChanged) {
    direction = 0; // Up
    directionChanged = true;
  }
}

bool checkPoint() {

  if (fruitRow == headRow && fruitCol == headCol) {
    tone(Buzzer, hertzWin, 500);
    return true;
  } else {
    return false;
  }

}

bool checkCollision() {

  for (int i = 1; i < bodyLength; i++) {
    if (bodyRow[i] == headRow && bodyCol[i] == headCol) {
      return true;
    }
  }
  return false;

}

void gameOver() { 

  tone(Buzzer, hertzLose, 1000);
  delay(2000);

  lc.clearDisplay(0);

  for(int g = 0; g < 2; g++){
    lc.setRow(0, 0, B01010101);
    lc.setRow(0, 1, B10101010);
    lc.setRow(0, 2, B01010101);
    lc.setRow(0, 3, B10101010);
    lc.setRow(0, 4, B01010101);
    lc.setRow(0, 5, B10101010);
    lc.setRow(0, 6, B01010101);
    lc.setRow(0, 7, B10101010);
    delay(400);
    lc.clearDisplay(0);
    lc.setRow(0, 0, B10101010);
    lc.setRow(0, 1, B01010101);
    lc.setRow(0, 2, B10101010);
    lc.setRow(0, 3, B01010101);
    lc.setRow(0, 4, B10101010);
    lc.setRow(0, 5, B01010101);
    lc.setRow(0, 6, B10101010);
    lc.setRow(0, 7, B01010101);
    delay(400);
    lc.clearDisplay(0);
  }

  delay(2000);

  bodyLength = 0;
  start = true;
  headRow = 4;
  headCol = 4;

  attraction();

}
