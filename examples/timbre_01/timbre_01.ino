// Primer round
//
// Este ejemplo muestra como parsear un documento JSON en una respuesta HTTP.
// Usa la librería WIFI para conectarse.
//
// La respuesta a una solicitud GET en
// colormono.com/lab/recreo/api/timbre/data.json
// debería devolver algo como:
// {
//  "mute": false,
//  "ringFor": "1000",
//  "wait": "10000",
//  "working": "true"
// }

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>

const char* wifi_ssid = "quale-privada";
const char* wifi_password = "quale2010pri";

const char* api_uri = "colormono.com";
const char* api_endpoint = "/lab/recreo/api/timbre/data";

int outputPin = 13; // GPIO13 - D7

void setup() {
  // Initialize Serial port
  Serial.begin(115200);
  while (!Serial) continue;

  // Output pin
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW);

  // Connect to WiFi network
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
  
  // Get data
  getData();
}

void getData() {
  WiFiClient client;
  
  // Connect to HTTP server
  client.setTimeout(1000);
  if (!client.connect(api_uri, 80)) {
    Serial.println(F("Connection failed"));
    return;
  }
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
  Serial.println(root["ringFor"].as<char*>());
  Serial.println(root["wait"].as<char*>());
  Serial.println(root["working"].as<char*>());

  long ringFor = root["ringFor"];
  long wait = root["wait"];
  bool working = root["working"];
  recreo(ringFor, wait, working);

  // Disconnect
  client.stop();
}

void ringOn() {
  digitalWrite(outputPin, HIGH);
}

void ringOff() {
  digitalWrite(outputPin, LOW);
}

void recreo(long ringFor, long wait, bool working) {
  Serial.println("Comienza el recreo");
  Serial.print("Working: ");
  Serial.println(working);
  ringOn();
  delay(ringFor);
  ringOff();
  delay(wait);
  Serial.println("Termina el recreo");
  
  // Start again
  getData();
}


void loop() {
  // not used in this example
}

// See also
// --------
//
// The website arduinojson.org contains the documentation for all the functions
// used above. It also includes an FAQ that will help you solve any
// serialization  problem.
// Please check it out at: https://arduinojson.org/
//
// The book "Mastering ArduinoJson" contains a tutorial on deserialization
// showing how to parse the response from Yahoo Weather. In the last chapter,
// it shows how to parse the huge documents from OpenWeatherMap
// and Weather Underground.
// Please check it out at: https://arduinojson.org/book/
