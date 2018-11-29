// Ejemplo: https://github.com/acrobotic/Ai_Tips_ESP8266/blob/master/simple_client/simple_client.ino

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "quale-privada";
const char* password = "quale2010pri";

int ledPin = 13; // GPIO13 - D7

ESP8266WebServer server;
#define REGLAS_URL "colormono.com/lab/recreo"
#define REGLAS_API "/api/timbre/data"

// espacio para guardar los datos recibidos
static char respBuffer[4096];

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", []() {
    server.send(200, "text/plain", "Hello World!");
  });
  server.on("/toggle", toggleLED);
  server.on("/getData", getData);
  server.begin();

}

void loop() {
  server.handleClient();
}

void toggleLED() {
  digitalWrite(ledPin, !digitalRead(ledPin));
  server.send(204, "");
}

WiFiClient client;

void getData() {
  const char request[] =
    "GET " REGLAS_API ".json HTTP/1.1\r\n"
    "User-Agent: ESP8266/0.1\r\n"
    "Accept: */*\n"
    "Host: " REGLAS_URL "\r\n"
    "Connection: close\r\n"
    "\r\n";
  Serial.println(request);
  client.connect(REGLAS_URL, 80);
  client.print(request);
  client.flush();
  delay(1000);

  uint16_t index = 0;
  while (client.connected())
  {
    if (client.available())
    {
      respBuffer[index++] = client.read();
      delay(1);
    }
  }
  client.stop();
  char * json = strchr(respBuffer, '{');

  DynamicJsonBuffer jBuffer;
  JsonObject& root = jBuffer.parseObject(json);
  JsonObject& current = root["recreo"];
  String duracion = current["duracion"];
  String msg = "Duracion: " + duracion;
  server.send(200, "text/plain", msg);
}
