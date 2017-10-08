/* Copyright (c) 2017 LeRoy Miller
 *  
 *  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses>
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <Wire.h>
#include "OLEDDisplayUi.h"
#include <NeoPixelBus.h> //https://github.com/Makuna/NeoPixelBus

SSD1306  display(0x3c, 5, 4);
OLEDDisplayUi ui     ( &display );

// Update these with values suitable for your network.

const char* ssid = "Motel6";
const char* password = "";
const char* mqtt_server = "iot.eclipse.org";

String color;
int p = 25; //Neopixel delay
#define NUM_LEDS 4
#define DATA_PIN 12
#define colorSaturation 10

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(NUM_LEDS, DATA_PIN);

#define white colorSaturation, colorSaturation, colorSaturation
#define warmwhite colorSaturation-2, 245, 230
#define oldlace colorSaturation-2,245,230 
#define black 0,0,0
#define red colorSaturation,0,0
#define green 0, colorSaturation, 0
#define blue 0,0,colorSaturation
#define cyan 0,colorSaturation,colorSaturation
#define magenta colorSaturation,0,colorSaturation
#define yellow colorSaturation,colorSaturation,0 //looks green/yellow to me
#define purple 102,51,204 //looks more blue to me (maybe 160, 32, 240)
#define orange colorSaturation,153,0 //looks more yellow to me
#define pink colorSaturation, 192, 203

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, String(millis()));
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
display->setTextAlignment(TEXT_ALIGN_LEFT);
display->setFont(ArialMT_Plain_16);
display->drawString(0+x, 10+y,"LeRoy Miller");
display->drawString(25+x, 40+y,"KD8BXP");
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  display->drawStringMaxWidth(0+x,0+y, 120, "Automation Technology      Club!");
  
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 20 + y, "kd8bxp.blogspot.com");
  display->drawString(0 + x, 30 + y, "github.com/kd8bxp");
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3 };

// how many frames are there?
int frameCount = 3;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

void setup() {
 
  ui.setTargetFPS(30);
  ui.setIndicatorPosition(BOTTOM);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_UP);
  ui.setFrames(frames, frameCount);
  ui.init();
  display.flipScreenVertically();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  strip.Begin();
  strip.Show();

}

void setup_wifi() {

  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  color = "";
  for (int i = 0; i < length; i++) {
     color+=(char)payload[i];
  }
  }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
   
    uint64_t chipid = ESP.getEfuseMac(); // MAC address of ESP32
    uint16_t chip = (uint16_t)(chipid>>32);
    char clientid[25];
    snprintf(clientid,25,"CheerlightGhost-%08X",chipid);
    if (client.connect(clientid)) {
            client.subscribe("cheerlights");
    } else {
            delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
int remainingTimeBudget = ui.update();

  setcolor();  
colorDisplay(black,p);
taskYIELD();
vTaskDelay(10);
}

void setcolor() {
  if (color == "white") {colorDisplay(white,p);
} else if (color == "warmwhite" || color == "oldlace" ) {colorDisplay(oldlace,p); //has a cyan color to me.
} else if (color == "black" || color == "off") {colorDisplay(black,p);
} else if (color == "red") { colorDisplay(red,p);
} else if (color == "green") { colorDisplay(green, p);
} else if (color == "blue") { colorDisplay(blue,p);
} else if (color == "cyan") {colorDisplay(cyan,p);
} else if (color == "magenta") {colorDisplay(magenta,p);
} else if (color == "yellow") {colorDisplay(yellow,p); //looks green/yellow to me
} else if (color == "purple") {colorDisplay(purple,p); //looks more blue to me (maybe 160, 32, 240)
} else if (color == "orange") {colorDisplay(orange,p); //looks more yellow to me
} else if (color == "pink") { colorDisplay(pink,p);}
//else {colorDisplay(0,0,0,p);}
}

void colorDisplay(uint16_t r, uint16_t g, uint16_t b, uint8_t wait) {
for (uint16_t i=0; i<NUM_LEDS; i++) {
RgbColor c(r,g,b);
strip.SetPixelColor(i, c);
strip.Show();
int remainingTimeBudget = ui.update();
if (remainingTimeBudget > 0) {
delay(wait);
//taskYIELD();
vTaskDelay(10);
}
}
}



