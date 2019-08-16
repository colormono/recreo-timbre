/*
  Actitud
  Timbre Rojo
  by COLORMONO <http://colormono.com>

  Prende, espera, apaga, loop

  Elements:
  * Board led
  * Analog relay
  * Analog button
  * 180deg micro servo
  * Display TM1637
*/
#include <Servo.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include "SevenSegmentFun.h"

// pins
const byte ledPin = 13;
const byte displayCLK = 9;
const byte displayDIO = 8;
const byte buttonPin = 7;
const byte servoPin = 6;
const byte relayPin = 4;

// config
SevenSegmentFun display(displayCLK, displayDIO);
Servo servo;

// Variables will change:
boolean buttonPressed = false; // check if button is pressed
int currentItem = 0;           // counter
int currentScore = 0;          // counter
int scoreToWin = 10;           // counter
int cursorPosition = 0;        // control display alignment
int minScore = 100;
int maxScore = 999;

// pomodoro
boolean pomodoro = false;     // check if button is pressed
int pressToPomodoro = 5000;   // milliseconds to keep button pressed
int pressCounter = 0;         // control display alignment
unsigned int clockSpeed = 60; // con 60 queda convertido a segundos
int minFocusTime = 20;
int maxFocusTime = 30;
int minRelaxTime = 512;
int maxRelaxTime = 1024;

// debounce
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
int buttonState;                    // the current reading from the input pin
int lastButtonState = LOW;          // the previous reading from the input pin
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 10;   // the debounce time; increase if the output flickers

void setup()
{
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  // initial states
  digitalWrite(relayPin, HIGH);
  display.begin();
  display.setBacklight(90); // Brillo 10% (100% en producción)
  display.print("HOLA");
  // display.print("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
  servo.attach(servoPin);
  servo.write(currentItem);
  ring();
  delay(1500);

  displayLoadAnim();
  startGame();
}

void loop()
{
  // listen to button state
  int reading = digitalRead(buttonPin);

  // If button changed, due to noise or pressing
  // reset the debouncing timer
  if (reading != lastButtonState)
    lastDebounceTime = millis();

  // if button is voluntary pressed
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    //Serial.println(reading);

    // if the button state has changed:
    if (reading != buttonState)
    {
      buttonState = reading;

      // COUNTDOWN
      // on button press
      // and is not pomodoro time
      if (buttonState == HIGH)
      {
        currentScore += 1;
        buttonPressed = true;

        if (currentScore < scoreToWin)
          updateScore();
        else
          fireworks();
      }

      // reset pomodoro listener
      if (buttonState == LOW)
      {
        pressCounter = 0;
        buttonPressed = false;
      }
    }

    // POMODORO
    // if button is pressed
    // and button is not changing
    if (buttonPressed)
    {
      pressCounter += 1;
      Serial.println(pressCounter);

      if (pressCounter > pressToPomodoro)
      {
        pomodoroFocus();
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}

void startGame()
{
  // level values
  scoreToWin = random(minScore, maxScore);
  currentScore = 0;

  display.clear();
  display.print("PLAY");
  delay(1000);
  displayLoadAnim();
  delay(10);
  updateScore();
}

void updateScore()
{
  int score = scoreToWin - currentScore;

  if (score > 999)
    cursorPosition = 0;
  else if (score > 99)
    cursorPosition = 1;
  else if (score > 9)
    cursorPosition = 2;
  else
    cursorPosition = 3;

  display.clear();
  display.setCursor(0, cursorPosition);
  display.print(score);
}

void fireworks()
{
  display.clear();
  display.snake(3);
  display.clear();
  ring();
  powerUp();

  unsigned int numMoves = random(minRelaxTime, maxRelaxTime);
  unsigned int timeDelay = 100;
  display.bouchingBall(numMoves, timeDelay);
  display.setBacklight(90);
  display.clear();
  ringDouble();

  startGame();
}

void powerUp()
{
  // pick a new random item, never the same!
  int newItem = random(0, 7);
  while (newItem == currentItem)
    newItem = random(0, 7);
  goToItem(newItem);
}

void goToItem(int item)
{
  servo.attach(servoPin);
  servo.write(item * 30);
  delay(2000);
  servo.detach();
}

void ring()
{
  digitalWrite(relayPin, LOW);
  delay(33);
  digitalWrite(relayPin, HIGH);
  delay(1500);
}

void ringDouble()
{
  digitalWrite(relayPin, LOW);
  delay(33);
  digitalWrite(relayPin, HIGH);
  delay(66);
  digitalWrite(relayPin, LOW);
  delay(33);
  digitalWrite(relayPin, HIGH);
  delay(1500);
}

void displayLoadAnim()
{
  display.clear();
  for (uint8_t i = 0; i <= 100; i += 25)
  {
    display.printLevelVertical(i);
    delay(100);
  };
  for (int8_t i = 100; i >= 0; i -= 25)
  {
    display.printLevelVertical(i);
    delay(100);
  }
}

void pomodoroFocus()
{
  display.clear();
  display.nightrider(3);
  display.clear();

  ring();
  goToItem(0); // Work icon

  byte hours = random(minFocusTime, maxFocusTime);
  byte min = 0;

  display.bombTimer(hours, min, clockSpeed, "    RECREO");
  display.setBacklight(90);
  display.clear();

  pomodoroRelax();
}

void pomodoroRelax()
{
  display.clear();

  ringDouble();
  powerUp(); // Random icon

  unsigned int numMoves = random(minRelaxTime, maxRelaxTime);
  unsigned int timeDelay = 100;
  display.bouchingBall(numMoves, timeDelay);
  display.setBacklight(90);
  display.clear();

  ringDouble();
  goToItem(3); // La NADA icon

  startGame();
}