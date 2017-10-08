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

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
 #include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
#include <Wire.h>
#include "OLEDDisplayUi.h"

SSD1306  display(0x3c, D1, D2);
// SH1106 display(0x3c, D3, D5);

OLEDDisplayUi ui     ( &display );

// Update these with values suitable for your network.

const char* ssid = "Motel6";
const char* password = "";
const char* mqtt_server = "iot.eclipse.org";

String color;
int brightness = 10;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, D6, NEO_GRB + NEO_KHZ800);

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
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  //Serial.begin(115200);
 ui.setTargetFPS(30);
 
// You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_UP);

  ui.setFrames(frames, frameCount);
  //ui.setOverlays(overlays, overlaysCount);
  ui.init();
  display.flipScreenVertically();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  strip.begin();
  strip.show();
  strip.setBrightness(brightness);
}

void setup_wifi() {

  delay(10);
   WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  //  Serial.print(".");
  }

}

void callback(char* topic, byte* payload, unsigned int length) {
  
  color = "";
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    color+=(char)payload[i];
  }
 }

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    uint32_t chipid=ESP.getChipId();
   
    char clientid[25];
    
    snprintf(clientid,25,"CheerlightGhost-%08X",chipid);
    if (client.connect(clientid)) {
            client.subscribe("cheerlights");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
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
//if (remainingTimeBudget > 0) {
  setcolor();  
colorDisplay(strip.Color(0,0,0),100);
//delay(remainingTimeBudget);
//}

}

void setcolor() {
  if (color == "white") {colorDisplay(strip.Color(255,255,255),100);
} else if (color == "warmwhite" || color == "oldlace" ) {colorDisplay(strip.Color(253,245,230),100); //has a cyan color to me.
} else if (color == "black" || color == "off") {colorDisplay(strip.Color(0,0,0),100);
} else if (color == "red") { colorDisplay(strip.Color(255,0,0),100);
} else if (color == "green") { colorDisplay(strip.Color(0, 255, 0), 100);
} else if (color == "blue") { colorDisplay(strip.Color(0,0,255),100);
} else if (color == "cyan") {colorDisplay(strip.Color(0,255,255),100);
} else if (color == "magenta") {colorDisplay(strip.Color(255,0,255),100);
} else if (color == "yellow") {colorDisplay(strip.Color(255,255,0),100); //looks green/yellow to me
} else if (color == "purple") {colorDisplay(strip.Color(102,51,204),100); //looks more blue to me (maybe 160, 32, 240)
} else if (color == "orange") {colorDisplay(strip.Color(255,153,0),100); //looks more yellow to me
} else if (color == "pink") { colorDisplay(strip.Color(255, 192, 203),100);}

}

void colorDisplay(uint32_t c, uint8_t wait) {
for (uint16_t i=0; i<strip.numPixels(); i++) {
strip.setPixelColor(i, c);
strip.show();

int remainingTimeBudget = ui.update();
if (remainingTimeBudget > 0) {
delay(wait);
}
}
}



