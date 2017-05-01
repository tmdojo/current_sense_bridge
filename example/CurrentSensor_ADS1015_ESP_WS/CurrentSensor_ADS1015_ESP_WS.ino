/*
current_sense_bridge example code

This code demonstrates how to read data from ADS1015 ADC on the current_sense_bridge board.

You need to install ADS1X15 Arduino library written by Adafruit Industries
https://github.com/adafruit/Adafruit_ADS1X15
Significant portion of this code was taken from the example included in the library.

This code is largely based on WebSocketServer_LEDcontrol.ino
https://github.com/Links2004/arduinoWebSockets

Run ESP-8266 with this sketch, access from your Mac or iPhone at this URL
http://esp-ct.local
You would see live update of data with plot

Author: Shunya Sato

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

// #include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "Timer.h"

#define USE_SERIAL Serial


ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
            break;
    }

}

Timer t;
//Adafruit_ADS1115 ads;
Adafruit_ADS1015 ads;

// const float multiplier = 3.0F; /* ADS1015  @ 2/3x gain +/- 6.144V (12-bit results) */
const float multiplier = 2.0F; /* ADS1015  @ 1x gain   +/- 4.096V (12-bit results) */
// const float multiplier = 1.0F; /* ADS1015  @ 2x gain   +/- 2.048V (12-bit results) */
// const float multiplier = 0.5F; /* ADS1015  @ 4x gain   +/- 1.024V (12-bit results) */
// const float multiplier = 0.25F; /* ADS1015  @ 8x gain   +/- 0.512V (12-bit results) */
// const float multiplier = 0.125F; /* ADS1015  @ 16x gain  +/- 0.256V (12-bit results) */

void readADC(){
  int16_t adc0;
  int16_t adc1;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);

  Serial.print(millis());
  Serial.print(", ");
  Serial.print(adc0);
  Serial.print(", ");
  Serial.print(adc0 * multiplier);
  Serial.print(", ");
  Serial.print(adc1);
  Serial.print(", ");
  Serial.println(adc1 * multiplier);

  String tmp = String(adc1*multiplier);

  webSocket.broadcastTXT(tmp);
  
}

void setup() {
    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP(“SSID", “password");
    WiFiMulti.addAP(“SSID2", “password");
    
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp-ct")) {
        USE_SERIAL.println("MDNS responder started");
    }

    // handle index
    server.on("/", []() {
        // send index.html
        server.send(200, "text/html", "<html><head><script type='text/javascript' src='https://cdnjs.cloudflare.com/ajax/libs/smoothie/1.27.0/smoothie.js'></script></head><body>Current sensor reading: <span id='nowvalue'></span><br/><br/><canvas id='mycanvas' width='400' height='100'></canvas><script>var smoothie = new SmoothieChart({  grid: { strokeStyle:'rgb(125, 125, 125)', fillStyle:'rgb(60, 0, 0)',          lineWidth: 1, millisPerLine: 250, verticalSections: 6, },  labels: { fillStyle:'rgb(255, 255, 0)' }});smoothie.streamTo(document.getElementById('mycanvas'), 100 /*delay*/);var line1 = new TimeSeries();setInterval(function() {  line1.append(new Date().getTime(), Math.random());}, 1000);smoothie.addTimeSeries(line1,  { strokeStyle:'rgb(0, 255, 0)', fillStyle:'rgba(0, 255, 0, 0.4)', lineWidth:3 });var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date());};connection.onerror = function (error) {  console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);  line1.append(new Date().getTime(), parseFloat(e.data));  document.getElementById('nowvalue').innerHTML = e.data;};</script></body></html>");
    });

    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);

  //Serial.println("Getting single-ended readings from AIN0..3");
  //Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!

  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  ads.begin();
  //ads.begin(4,5);
  /* Use this to set data rate for the 16-bit version (optional)*/
  //ads.setSPS(ADS1115_DR_860SPS);                      // for ADS1115 fastest samples per second is 860 (default is 128)

  // t.every(10, readADC); // 10ms interval = 100Hz
  // t.every(20, readADC); // 20ms interval = 50Hz
  t.every(100, readADC); // 100ms interval = 10Hz
}

void loop() {
    webSocket.loop();
    server.handleClient();
    t.update();
}

