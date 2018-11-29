/*
  Reloj round 1
  Cuenta regresiva

  The circuit:
  * connect TM1637 pin CLK to Arduino pin D4
  * connect TM1637 pin DIO to Arduino pin D5
  * connect TM1637 pin Vcc to Arduino pin 5V
  * connect TM1637 pin GND to Arduino pin GND
*/
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include "SevenSegmentFun.h"

const byte PIN_CLK = 4;
const byte PIN_DIO = 5;
SevenSegmentFun display(PIN_CLK, PIN_DIO);

// run setup code
void setup() {
  Serial.begin(115200);
  display.begin();
  display.setBacklight(10);  // Brillo 10% (100% en producción)
  delay(1000);
};

// run loop (forever)
void loop() {

  // recreo
  display.print("PLAY");
  delay(5000);
  unsigned int numMoves = 100; 
  unsigned int timeDelay = 100;
  display.bouchingBall(numMoves, timeDelay);

  // trabajo
  display.print("CODE");
  delay(3000); // Este debería durar lo mismo que el timbre?
  byte hours = 5; 
  byte min = 16; 
  unsigned int speed = 1000; // Con 100 queda convertido a segundos
  // Y si pongo palabras aleatorias que tengan que ver?
  display.bombTimer(hours, min, speed, "SEND");
  delay(3000);
};
