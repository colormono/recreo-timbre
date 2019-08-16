/* Servo
  by COLORMONO <http://colormono.com>
  This example code is in the public domain.

  Rotate X deg when a button is pressed
  X=360/N;

  Help: https://www.arduino.cc/en/Reference/Servo
*/

#include <Servo.h>

Servo itemBox;

const int servoPin = 6;     // servo pin
const int buttonPin = 7;    // button pin
int buttonState = 0;        // track button state
int wait = 1000;             // time interval between items
int pos = 180;
String readString;

void setup() {
  Serial.begin(9600);
  Serial.println("serial servo incremental test code");
  Serial.println("type a character (s to increase or a to decrease)");
  Serial.println("and enter to change servo position");
  Serial.println("use strings like 90x for new servo position in deg");
  Serial.println();

  itemBox.attach(servoPin);

  pinMode(buttonPin, INPUT);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the string readString
    delay(2);  //slow looping to allow buffer to fill with next character
  }
  if (readString.length() > 0) {
    if (readString.indexOf('x') > 0) {
      Serial.println(pos);
      pos = readString.toInt();
    }
  }
  readString = ""; //empty for next input


  buttonState = digitalRead(buttonPin);
  //Serial.print("Button is ");
  //Serial.println(buttonState);

  if (buttonState == HIGH) {
    Serial.println("arranca");
    itemBox.attach(servoPin);
    itemBox.write(pos);
    delay(wait);
    itemBox.detach();

    //itemBox.attach(servoPin);
    //itemBox.writeMicroseconds(1170);
    delay(wait);
    //itemBox.detach();

    itemBox.attach(servoPin);
    itemBox.write(0);
    delay(wait);
    itemBox.detach();

  } else {
    itemBox.detach();
  }
}

