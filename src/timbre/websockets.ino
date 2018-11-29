/**
   METODOS PARA COMUNICACIÓN VIA WEBSOCKETS
*/

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {

    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      isConnected = false;
      break;

    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n",  payload);
        isConnected = true;
        // send message to server when Connected
        // socket.io upgrade confirmation message (required)
        webSocket.sendTXT("5");
      }
      break;

    case WStype_TEXT:
      {
        Serial.printf("Payload: %s\n", payload);
        String text = ((const char *)&payload[0]);
        int pos = text.indexOf('{');
        String msg = text.substring(pos, text.length() - 1);

        // Llave a controlar
        int keyPosInit = text.indexOf('"') + 1;
        int keyPosEnd = text.indexOf('"', keyPosInit);
        String key = text.substring(keyPosInit, keyPosEnd);
        Serial.println("Key: " + key);

        // Analizar payload según clave
        /*
          switch (key) {
          case "led":
            //Hacer algo sólo para el led
            break;
          default:
            break;
          }
        */
        parseMsg(msg);

        // send message to server
        // webSocket.sendTXT("message here");
      }
      break;

    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
  }

}

void parseMsg(String msg) {
  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(7) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonObject& root = jsonBuffer.parseObject(msg);
  if (!root.success()) {
    Serial.println(F("Parsing failed!"));
    return;
  }

  // Fetch values

  // Estado
  const char* estado = root["status"];
  Serial.print("estado: ");
  Serial.println(estado);
  if ( strcmp(estado, "sonando") == 0 ) {
    ringOn();
  } else {
    ringOff();
  }

  // Energía
  long energy = root["energy"];
  Serial.print("energy: ");
  Serial.println(energy);

  // Tempo
  long tempo = root["tempo"];
  Serial.print("tempo: ");
  Serial.println(tempo);

  /*
    // Palabra Clave
    String palabraClave = root["palabraClave"];
    Serial.print("palabraClave: ");
    Serial.println(palabraClave);
    display.print(palabraClave);
  */

  /*
    // Paleta
    int r = root["palette"][0]["r"];
    int g = root["palette"][0]["g"];
    int b = root["palette"][0]["b"];
    analogWrite(LED_RED,    r);
    analogWrite(LED_GREEN,  g);
    analogWrite(LED_BLUE,   b);
    Serial.print("r: ");
    Serial.println(r);
    Serial.print("g: ");
    Serial.println(g);
    Serial.print("b: ");
    Serial.println(b);
  */
}
