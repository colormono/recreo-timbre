/**
   METODOS PARA EL WEBSERVER
*/

void setupServer() {
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.on("/demo", handleDemo);
  server.on("/pomodoro", handlePomodoro);
  server.on("/testing", handleTesting);
  server.on("/testRelay", testRelay);
  server.on("/testDisplay", testDisplay);
  server.on("/getADC", handleADC); //Reads ADC function is called from out index.html
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

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  // Store values for later use.
  io_server = json["io_server"].as<String>();
  object_mode = json["object_mode"];
  const char* io_server_port = json["io_server_port"];
  Serial.print("Loaded io_server: ");
  Serial.println(io_server);
  Serial.print("Loaded io_server_port: ");
  Serial.println(io_server_port);
  Serial.print("Loaded object_mode: ");
  Serial.println(object_mode);

  return true;
}

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

// --- RUTAS ---

void handleADC() {
  int a = analogRead(A0);
  a = map(a, 0, 1023, 0, 100);
  String adc = String(a);
  Serial.println(adc);
  server.send(200, "text/plane", adc);
}

void handleRoot() {
  server.sendHeader("Location", "/index.html", true);
  server.send(302, "text/plane", "");
}

void handleTesting() {
  server.sendHeader("Location", "/testing.html", true);
  server.send(302, "text/plane", "");
}

void handlePomodoro() {
  server.sendHeader("Location", "/pomodoro.html", true);
  server.send(302, "text/plane", "");
}

// --- TESTS ---

void testRelay() {
  int value = server.arg("value").toInt();
  if ( value == 1) {
    ringOn();
  } else {
    ringOff();
  }
  server.send(200, "text/plane", String(value));
}

void testDisplay() {
  int value = server.arg("value").toInt();
  if ( value == 0) {
    setDisplayLoading();
  } else if ( value == 1) {
    setDisplayPlay();
  } else if ( value == 2 ) {
    setDisplayCode();
  } else {
    display.print("COLORMONO");
  }
  server.send(200, "text/plane", String(value));
}

void handleRoot2() {
  //server.send(200, "text/plain", "hello world from esp8266!");
  //server.send(200, "text/html", body + msg + footer);

  // Led
  int ledState = server.arg("led").toInt();
  if ( ledState == 1) {
    setLedSuccess();
  } else if ( ledState == 2) {
    setLedWarning();
  } else if ( ledState == 3 ) {
    setLedError();
  }

  // Led custom color
  int ledR = server.arg("r").toInt();
  int ledG = server.arg("g").toInt();
  int ledB = server.arg("b").toInt();
  if ( ledR && ledG && ledB ) {
    setColor(ledR, ledG, ledB);
  }

  // Pomodoro
  int pomodoro = server.arg("pomodoro").toInt();
  if ( pomodoro == 1 ) {
    startFocus(25);
  } else if ( pomodoro == 2 ) {
    startBreak(5);
  }
}

void handleSave() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  json["io_server"] = server.arg("io_server");
  json["io_server_port"] = server.arg("io_server_port");
  json["object_mode"] = server.arg("object_mode");

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //msg = "Failed to open config file for writing";
  } else {
    //msg = "Configuracion Guardada...";
  }

  json.printTo(configFile);
  //server.send(200, "text/html", body + msg + footer);

  // restart
  //ESP.restart();
}

void handleDemo() {
  int timePlay = server.arg("timePlay").toInt();
  int timeWork = server.arg("timeWork").toInt();
  startRecreo(timePlay, timeWork);
  //server.send(200, "text/html", body + msg + footer);
}

