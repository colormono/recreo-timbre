/* Servo
  by COLORMONO <http://colormono.com>
  This example code is in the public domain.

  Rotate X deg when a button is pressed
  X=360/N;

  Help: https://www.arduino.cc/en/Reference/Servo

  Valores
  1170 = detenido
*/

#include <Servo.h>

Servo itemBox;

const int servoPin = 6;     // servo pin
const int buttonPin = 7;    // button pin
int buttonState = 0;        // track button state
int wait = 1000;             // time interval between items

void setup() {
  Serial.begin(9600);

  itemBox.attach(servoPin, 544, 2400); //servo control pin, and range if desired
  itemBox.writeMicroseconds(1500);  // set servo to mid-point

  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  //Serial.print("Button is ");
  //Serial.println(buttonState);

  //if (buttonState == HIGH) {
    Serial.println("arranca");
    itemBox.attach(servoPin);
    itemBox.writeMicroseconds(110);
    delay(wait);
    itemBox.detach();

    //itemBox.attach(servoPin);
    //itemBox.writeMicroseconds(1170);
    delay(wait);
    //itemBox.detach();

    itemBox.attach(servoPin);
    itemBox.writeMicroseconds(1200);
    delay(wait);
    itemBox.detach();

  //} else {
    //itemBox.detach();
  //}
}

