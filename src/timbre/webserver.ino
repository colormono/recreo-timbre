/**
   METODOS PARA EL WEBSERVER
*/

void setupServer() {
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.on("/demo", handleDemo);
  server.begin();
  Serial.println("HTTP server started");
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


// --- TEMPLATE HTML ---
// TODO
// - host styles on local server ()

String msg = "";

String body = "<!DOCTYPE html>"
              "<html>"
              "<head>"
              "<title>Timbre Config</title>"
              "<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'>"
              "<meta charset='UTF-8'>"
              "<style>*{text-align:center;font-family:helvetica,arial;}"
              ".btn{display:block;padding:10px;margin:10px;background-color:#00ccff;color:#000000;text-decoration:none;border:none;}"
              "input{width:100%;margin:10px;padding:10px;}</style>"
              "</head>"
              "<body>"
              "<h4>Pomodoro</h4>"
              "<a class='btn' href='/?pomodoro=1'>Pomodoro Focus</a> "
              "<a class='btn' href='/?pomodoro=2'>Pomodoro Break</a> "
              "<h4>Device</h4>"
              "<a class='btn' href='/demo?timePlay=20&timeWork=1'>Recreo Demo</a> "
              "<h4>Relay</h4>"
              "<a class='btn' href='/?relay=1'>Relay ON</a> "
              "<a class='btn' href='/?relay=2'>Relay OFF</a> "
              "<h4>Led</h4>"
              "<a class='btn' href='/?led=1'>Success</a> "
              "<a class='btn' href='/?led=2'>Warning</a> "
              "<a class='btn' href='/?led=3'>Error</a> "
              "<a class='btn' href='/?r=255&g=100&b=120'>Custom color</a> "
              "<h4>Display</h4>"
              "<a class='btn' href='/?display=1'>Loading</a> "
              "<a class='btn' href='/?display=2'>Play</a> "
              "<a class='btn' href='/?display=3'>Code</a> "
              "<hr />"
              "<h4>Setup</h4>"
              "<form action='save' method='get'>"
              "<p>io_server:<br>"
              "<input class='input' name='io_server' type='text' placeholder='dato actual'></p>"
              "<p>io_server_port:<br>"
              "<input class='input' name='io_server_port' type='text' placeholder='dato actual'></p>"
              "<p>modo:<br>"
              "<input class='input' name='object_mode' type='number' placeholder='dato actual'></p>"
              "<p><input class='btn' type='submit' value='GUARDAR'/></p>"
              "</form>";

String footer = "</body>"
                "</html>";


// --- RUTAS ---

void handleRoot() {
  //server.send(200, "text/plain", "hello world from esp8266!");
  server.send(200, "text/html", body + msg + footer);

  // Relay
  int relayState = server.arg("relay").toInt();
  if ( relayState == 1) {
    ringOn();
  } else if ( relayState == 2) {
    ringOff();
  }

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

  // Display
  int displayState = server.arg("display").toInt();
  if ( displayState == 1) {
    setDisplayLoading();
  } else if ( displayState == 2) {
    setDisplayPlay();
  } else if ( displayState == 3 ) {
    setDisplayCode();
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
    msg = "Failed to open config file for writing";
  } else {
    msg = "Configuracion Guardada...";
  }

  json.printTo(configFile);
  server.send(200, "text/html", body + msg + footer);

  // restart
  //ESP.restart();
}

void handleDemo() {
  int timePlay = server.arg("timePlay").toInt();
  int timeWork = server.arg("timeWork").toInt();
  startRecreo(timePlay, timeWork);
  server.send(200, "text/html", body + msg + footer);
}

