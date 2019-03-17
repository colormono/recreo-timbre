/**
   METODOS PARA COMUNICACIÓN VIA WEBSOCKETS
*/

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {

    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      isConnected = false;
      setLedError();
      break;

    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n",  payload);
        isConnected = true;
        // send message to server when Connected
        // socket.io upgrade confirmation message (required)
        webSocket.sendTXT("5");
        // SendHelloToServer
        webSocket.sendTXT("42[\"device:connected\",{\"device\":\"timbre\"}]");
      }
      break;

    case WStype_TEXT:
      {
        // listenToServer
        Serial.printf("Payload: %s\n", payload);
        String text = ((const char *)&payload[0]);
        int pos = text.indexOf('{');
        String msg = text.substring(pos, text.length() - 1);

        // Nombre del evento
        int keyPosInit = text.indexOf('"') + 1;
        int keyPosEnd = text.indexOf('"', keyPosInit);
        String key = text.substring(keyPosInit, keyPosEnd);
        Serial.println("Evento: " + key);

        // Analizar payload según evento
        if (key == "update:timbre" ) {
          parseMsg(msg);
        }
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
  // Allocate JsonBuffer using arduinojson.org/assistant to compute the capacity
  const size_t capacity = JSON_OBJECT_SIZE(7) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonObject& root = jsonBuffer.parseObject(msg);
  if (!root.success()) {
    Serial.println(F("Parsing failed!"));
    setLedError();
    return;
  }

  // Fetch values
  int led = root["led"];
  Serial.print("led: ");
  Serial.println(led);
  if (led == 1) {
    setLedWarning();
  }
  else if (led == 2) {
    setLedSuccess();
  }
  else if (led == 3) {
    setLedError();
  }

  int relay = root["relay"];
  Serial.print("relay: ");
  Serial.println(relay);
  if (relay == 1) {
    ringOn();
  }
  else if (relay == 2) {
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
    // Estado
    const char* estado = root["status"];
    Serial.print("estado: ");
    Serial.println(estado);
    if ( strcmp(estado, "sonando") == 0 ) {
      ringOn();
    } else {
      ringOff();
    }

    // Palabra Clave
    String palabraClave = root["palabraClave"];
    Serial.print("palabraClave: ");
    Serial.println(palabraClave);
    display.print(palabraClave);
  */
}
