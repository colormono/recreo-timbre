/* Boton con debounce
  by COLORMONO <http://colormono.com>
  This example code is in the public domain.

  Al presionar comienza cuenta regresiva
  mientras se mantiene apretado la cuenta disminuye
  si se suelta antes de ser meno o igual a cero, reinicia
  si llega al final enciende led
  espera x segundos, apaga el led y
  vuelve a habilitar el boton
*/
const int buttonPin = 7;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

// Variables will change:
boolean playing = false;    // check if button is pressed
int currentScore = 0;       // counter
int scoreToWin = 2000;       // counter

// debounce
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
int buttonState;                     // the current reading from the input pin
int lastButtonState = LOW;           // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
// when playing increase to 150 this is like the level complexity

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, LOW);
}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // esto lo escucha siempre
    //Serial.println(reading);

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // start game
      if (buttonState == HIGH && !playing) {
        Serial.println("START GAME");
        currentScore = 0;
        playing = true;
      }

      if (buttonState == LOW && playing) {
        Serial.println("GAME OVER");
        for (int i = 0; i < 5; i++) {
          digitalWrite(ledPin, HIGH);
          delay(100);
          digitalWrite(ledPin, LOW);
          delay(100);
        }
        playing = false;
      }
    }

    // playing
    if (playing) {
      currentScore = currentScore + 1;
      Serial.print("Score ");
      Serial.println(currentScore);

      // win
      if (currentScore > scoreToWin) {
        // animate or do whatever
        Serial.println("WIN");
        for (int i = 0; i < 30; i++) {
          digitalWrite(ledPin, HIGH);
          delay(100);
          digitalWrite(ledPin, LOW);
          delay(100);
        }
        Serial.println("RESTART");
        playing = false;
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
