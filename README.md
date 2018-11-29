# Timbre

Marcar el inicio y el fin del recreo.
**Timbre de escuela, arduino.**

## Introducción

Suena cada vez que se refresca el patrón de reglas.
Indica el fin del recreo y el comienzo del siguiente.
Al sonar el timbre todo se apaga y vuelve a encenderse.
O se mete Glitch en las salidas.

**[Documentación API](#nombre)**

## Diagrama

![Columpio v1](docs/workflow.jpg)

## Circuito

Falta poner mi circuito final

![alt text](./images/relay-open.png 'Circuito normal abierto')
![alt text](./images/4digit-clock.jpg 'Circuito normal abierto')
![alt text](./images/external-supply.png 'External supply')

## Código

Descripción de algunos de los componentes escenciales del código.

### Configuración

- When your ESP starts up, it sets it up in Station mode and tries to connect to a previously saved Access Point
- If this is unsuccessful (or no previous network saved) it moves the ESP into Access Point mode and spins up a DNS and WebServer **(default ip 192.168.4.1)**
- Using any wifi enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point
- Because of the Captive Portal and the DNS server, you will either get a 'Join to network' type of popup or get any domain you try to access redirected to the configuration portal
- Choose one of the access points scanned, enter a password, click save
- ESP will try to connect. If successful, it relinquishes control back to your app. If not, reconnect to AP and reconfigure.

```
// Wifi
const char* wifi_ssid = "quale-privada";
const char* wifi_password = "quale2010pri";

// Server
const char* io_server = "192.168.1.107";
uint64_t io_server_port = 3000;
```

When your ESP starts up, it sets it up in Station mode and tries to connect to a previously saved Access Point
If this is unsuccessful (or no previous network saved) it moves the ESP into Access Point mode and spins up a DNS and WebServer (default ip 192.168.4.1)
Using any wifi enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point
Because of the Captive Portal and the DNS server, you will either get a 'Join to network' type of popup or get any domain you try to access redirected to the configuration portal
Choose one of the access points scanned, enter a password, click save
ESP will try to connect. If successful, it relinquishes control back to your app. If not, reconnect to AP and reconfigure.

### Correr servidor

### Correr cliente

### Instalar drivers

- [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
- [Driver CH340 MacOS](https://www.geekfactory.mx/download/driver-ch340-macos/) - Driver para dispositivos chinos

### Librerías requeridas

- [Arduino ESP8266](https://github.com/esp8266/Arduino) - Comunicarse con ESP8266
- [Arduino WebSockets](https://github.com/Links2004/arduinoWebSockets) - Trabajar con WebSockets desde Arduino. Usar la versión 5.\*, no la 6 porque está en beta. Prestar especial atención porque Arduino instala por defecto la 6.
- [Arduino JSON](https://arduinojson.org/) - Manipular documentos JSON
- [Arduino SevenSegmentTM1637](https://github.com/bremme/arduino-tm1637) - Controlar displays de 7 segmentos
- [Arduino WiFiManager](https://github.com/tzapu/WiFiManager) - You can connect to it using a phone or other WiFi device, default ip 192.168.4.1
- [Arduino ESP8266 filesystem uploader](https://github.com/esp8266/arduino-esp8266fs-plugin) - Arduino plugin for uploading files to ESP8266 file system

### Ejemplos

En el directorio `/examples` se encuentran sketchs con posibles soluciones para los ditstintos problemas, atacándolos por separado.

## Enlaces útiles

- [Quick start to Nodemcu esp8266](http://www.instructables.com/id/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/) - Como instalar los drivers en la Ide Arduino y controlar un LED desde el navegador.
- [Nodemcu dev kit using Arduino IDE: Get started with ESP8266](https://www.amazon.com/Nodemcu-dev-kit-using-Arduino-ebook/dp/B01A1R31K2/) - Libro de referencia
  (https://www.norwegiancreations.com/2017/09/arduino-tutorial-using-millis-instead-of-delay/)
- [ESP8266 Web Client: Accessing API's Using Arduino IDE](https://www.youtube.com/watch?v=8xqgdXvn3yw) - Video tutorial
- [Arduino Time](http://playground.arduino.cc/Code/Time) - Librería con funciones para el control de tiempo
- [Arduino Tutorial: Using millis() Instead of delay()]
- [Control del tiempo en NODE](https://nodejs.org/en/docs/guides/timers-in-node/)
- [NodeMCU RGB LED on Arduino IDE](https://roboindia.com/tutorials/RGB-LED-nodemcu)
- [SEVEN PRO TIPS FOR ESP8266](https://www.instructables.com/id/ESP8266-Pro-Tips/)
- [ESP8266 – SPIFFS + JSON](http://www.sinaptec.alomar.com.ar/2017/06/tutorial-8-esp8266-spiffs-json.html)
- [Build an ESP8266 Web Server – Code and Schematics](https://randomnerdtutorials.com/esp8266-web-server/)
