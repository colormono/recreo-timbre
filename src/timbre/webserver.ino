/**
   METODOS PARA EL WEBSERVER
*/

void setupServer() {
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.on("/demo", handleDemo);
  server.on("/testing", handleTesting);
  server.on("/testRelay", testRelay);
  server.on("/testLed", testLed);
  server.on("/testDisplay", testDisplay);
  server.on("/pomodoro", handlePomodoro);
  server.on("/reset", handleReset);
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI
  server.begin();
  Serial.println("HTTP server started");
  delay(1000);
}

void handleWebRequests() {
  if (loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

//bool loadConfig() {
//  File configFile = SPIFFS.open("/config.json", "r");
//  if (!configFile) {
//    Serial.println("Failed to open config file");
//    return false;
//  }
//
//  size_t size = configFile.size();
//  if (size > 1024) {
//    Serial.println("Config file size is too large");
//    return false;
//  }
//
//  // Allocate a buffer to store contents of the file.
//  std::unique_ptr<char[]> buf(new char[size]);
//
//  // We don't use String here because ArduinoJson library requires the input
//  // buffer to be mutable. If you don't use ArduinoJson, you may as well
//  // use configFile.readString instead.
//  configFile.readBytes(buf.get(), size);
//
//  StaticJsonBuffer<200> jsonBuffer;
//  JsonObject& json = jsonBuffer.parseObject(buf.get());
//
//  if (!json.success()) {
//    Serial.println("Failed to parse config file");
//    return false;
//  }
//
//  // Store values for later use.
//  reglas_server = json["reglas_server"].as<String>();
//  object_mode = json["object_mode"];
//  const char* reglas_port = json["reglas_port"];
//  Serial.print("Loaded reglas_server: ");
//  Serial.println(reglas_server);
//  Serial.print("Loaded reglas_port: ");
//  Serial.println(reglas_port);
//  Serial.print("Loaded object_mode: ");
//  Serial.println(object_mode);
//
//  return true;
//}

bool loadFromSpiffs(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) path += "index.htm";

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}

// --- SETUP ---

void handleRoot() {
  server.sendHeader("Location", "/index.html", true);
  server.send(302, "text/plane", "");
}

void handleReset() {
  server.sendHeader("Location", "/reset.html", true);
  server.send(302, "text/plane", "RESET");
  delay(1000);
  WiFi.disconnect(true);
  delay(2000);
  ESP.reset();
}

void handleSave() {
  String msg;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  json["reglas_server"] = server.arg("reglas_server");
  json["reglas_port"] = server.arg("reglas_port");
  json["object_mode"] = server.arg("object_mode").toInt();

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    msg = "Error al abrir el archivo de configuración";
  } else {
    msg = "Configuracion Guardada!";
  }

  json.printTo(configFile);
  server.send(200, "text/plane", msg);

  // restart
  //ESP.restart();
  delay(1000);
  ESP.reset();
  delay(5000);
}

// --- TESTING ---

void handleTesting() {
  server.sendHeader("Location", "/testing.html", true);
  server.send(302, "text/plane", "");
}

void handleDemo() {
  int timePlay = server.arg("timePlay").toInt();
  int timeWork = server.arg("timeWork").toInt();
  startRecreo(timePlay, timeWork);
  //server.send(200, "text/html", body + msg + footer);
}

void testRelay() {
  int value = server.arg("value").toInt();
  if ( value == 1) {
    ringOn();
  } else {
    ringOff();
  }
  server.send(200, "text/plane", String(value));
}

void testLed() {
  int value = server.arg("value").toInt();
  if ( value == 1) {
    setLedSuccess();
  } else if ( value == 2) {
    setLedWarning();
  } else if ( value == 3 ) {
    setLedError();
  }
  // Led custom color
  int ledR = server.arg("r").toInt();
  int ledG = server.arg("g").toInt();
  int ledB = server.arg("b").toInt();
  if ( ledR && ledG && ledB ) {
    setColor(ledR, ledG, ledB);
  }
  server.send(200, "text/plane", String(value));
}

void testDisplay() {
  int value = server.arg("value").toInt();
  if ( value == 0) {
    setDisplayLoading();
    display.clear();
  } else if ( value == 1) {
    setDisplayPlay();
    display.clear();
  } else if ( value == 2 ) {
    setDisplayCode();
    display.clear();
  } else {
    display.print("COLORMONO");
  }
  server.send(200, "text/plane", String(value));
}

// --- POMODORO ---

void handlePomodoro() {
  server.sendHeader("Location", "/pomodoro.html", true);
  server.send(302, "text/plane", "");
}

void handleRoot2() {
  //server.send(200, "text/plain", "hello world from esp8266!");
  //server.send(200, "text/html", body + msg + footer);

  // Pomodoro
  int pomodoro = server.arg("pomodoro").toInt();
  if ( pomodoro == 1 ) {
    startFocus(25);
  } else if ( pomodoro == 2 ) {
    startBreak(5);
  }
}


