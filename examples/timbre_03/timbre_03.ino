/*
  Timbre: round 3
  - Se conecta al wifi
  - Usa un led RGB como testigo
  - Consume un JSON vía HTTP con las reglas
  - Display de 7 digitos con cuentas regresivas
  - Relay para el timbre

  Comentario: El tiempo debería llegar en millis.

  API URL: colormono.com/lab/recreo/api/timbre/data.json

  Circuito:
  connect Relay pin CLK to Arduino pin D4
  connect Relay pin DIO to Arduino pin D5
  connect Relay pin Vcc to Arduino pin 5V
  connect Display pin CLK to Arduino pin D4
  connect Display pin DIO to Arduino pin D5
  connect Display pin Vcc to Arduino pin 5V
  connect Display pin GND to Arduino pin GND
  connect Led pin R to Arduino pin D1
  connect Led pin R to Arduino pin D2
  connect Led pin R to Arduino pin D3
  connect Led pin Catodo to Arduino pin GND

*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>

// Wifi
const char* wifi_ssid = "quale-privada";
const char* wifi_password = "quale2010pri";

// API
const char* api_uri = "colormono.com";
const char* api_endpoint = "/lab/recreo/api/timbre/data";

// Outputs
const byte PIN_DIO = 5; // D1
const byte PIN_CLK = 4; // PWM D2
const int PIN_R = 14; // PWM D5
const int PIN_G = 12; // PWM D6
const int PIN_B = 15; // PWM D8
const int PIN_RELAY = 13; // D7

// Defaults
bool MUTE = false;
long ringFor = 5000;
long playFor = 20000;
long workFor = 10000;

SevenSegmentFun display(PIN_CLK, PIN_DIO);

void setup() {
  // Initialize Serial port
  Serial.begin(115200);
  while (!Serial) continue;

  // Relay
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  // Led Testigo
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  setLedOff();

  // Display (Brillo al 10%, usar 100% en producción)
  display.begin();
  display.setBacklight(10);
  display.print("LOAD");

  // Connect to WiFi network
  setLedColor(255, 0, 0);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  setLedOff();

  // Get data
  getData();
}

void getData() {
  Serial.println("Getting data...");
  display.print("LOAD");
  WiFiClient client;

  // Connect to HTTP server
  client.setTimeout(1000);
  if (!client.connect(api_uri, 80)) {
    Serial.println(F("Connection failed"));
    setLedColor(255, 165, 0);
    delay(1000);
    getData();
    return;
  }
  setLedOff();
  Serial.println(F("Connected!"));

  // Send HTTP request
  client.println(F("GET /lab/recreo/api/timbre/data.json HTTP/1.0"));
  client.println(F("Host: colormono.com"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonBuffer jsonBuffer(capacity);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(client);
  if (!root.success()) {
    Serial.println(F("Parsing failed!"));
    return;
  }

  // Extract values
  Serial.println(F("Response:"));
  Serial.println(root["mute"].as<char*>());
  Serial.println(root["start"].as<char*>());
  Serial.println(root["ringFor"].as<char*>());
  Serial.println(root["playFor"].as<char*>());
  Serial.println(root["workFor"].as<char*>());

  bool isMute = root["mute"];
  bool start = root["start"];
  ringFor = root["ringFor"];
  playFor = root["playFor"];
  workFor = root["workFor"];

  // Disconnect
  client.stop();

  // Set output
  setOutput(isMute);

  // Start
  code(workFor);
}

// Comienza el trabajo
void code(long workFor) {
  Serial.println("Start coding...");

  // Suena el timbre
  ring(ringFor);
  //delay(ringFor);

  // Imprime CODE por 5 segundos
  display.print("CODE");
  delay(5000);

  // Cuenta regresiva y luego SEND por 5 segundos
  // Con una velocidad de 100 se comporta como segundos
  // Falta convertir workFor a minutos y segundos
  byte minutos = 5;
  byte segundos = 16;
  unsigned int speed = 1000;
  display.bombTimer(minutos, segundos, speed, "SEND");
  delay(5000);

  // LLama a recreo
  play(playFor);
}

// recreo
void play(long playFor) {
  Serial.println("Start playing...");

  // Suena el timbre
  ring(ringFor);
  delay(ringFor);

  // Imprime PLAY por 5 segundos
  display.print("PLAY");
  delay(5000);

  // Juega en la pantalla durante el recreo
  unsigned int numMoves = (int) playFor / 100;
  unsigned int timeDelay = 100;
  display.bouchingBall(numMoves, timeDelay);

  // Tocar el timbre sutilmente
  ring(200);
  delay(200);

  // Enviar mensaje
  sendToServer();
  byte minutos = 5;
  byte segundos = 16;
  unsigned int speed = 1000;
  display.bombTimer(minutos, segundos, speed, "SEND");
  delay(5000);
}

// Avisar al servidor que terminó y recomenzar
void sendToServer() {
  // Falta avisar que terminó
  Serial.print("Sending to server...");
  delay(5000);
  getData();
}

// Definir output
void setOutput(bool mute) {
  if (mute) {
    setLedColor(0, 255, 0);
    MUTE = true;
  } else {
    setLedOff();
    MUTE = false;
  }
}

// Encender led en color
void setLedColor(int r, int g, int b) {
  analogWrite(PIN_R, r);
  analogWrite(PIN_R, g);
  analogWrite(PIN_B, b);
}

// Apagar led
void setLedOff() {
  digitalWrite(PIN_R, LOW);
  digitalWrite(PIN_G, LOW);
  digitalWrite(PIN_B, LOW);
}

// Sonar por un tiempo
void ring(long tiempo) {
  Serial.print("Ring for: ");
  Serial.println(tiempo);
  if (MUTE) {
    while (tiempo > 0) {
      setLedColor(0, 255, 0);
      delay(500);
      setLedOff();
      delay(500);
      tiempo = tiempo - 1000;
    }
    setLedOff();
  } else {
    ringOn();
    delay(tiempo);
    ringOff();
  }
}

// Prender alarma
void ringOn() {
  if (!MUTE) {
    digitalWrite(PIN_RELAY, HIGH);
  }
}

// Apagar alarma
void ringOff() {
  digitalWrite(PIN_RELAY, LOW);
}

void loop() {
  // not used
}
