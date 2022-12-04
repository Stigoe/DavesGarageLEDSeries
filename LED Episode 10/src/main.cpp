//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        LED Episode 08
//
// Description:
//
//   Draws sample effects on a an addressable strip using FastLED
//
// History:     Sep-15-2020     davepl      Created
//              Oct-05-2020     davepl      Revised for Episode 07
//              Oct-11-2020     davepl      Revised for Episode 08
//              Oct-16-2020     davepl      Revised for Episode 09
//---------------------------------------------------------------------------


#include <Arduino.h>            // Arduino Framework
#include <Wifi.h>
#include <MQTT.h>
#include <../include/secrets.h>

#define FASTLED_INTERNAL        // Suppress build banner
#include <FastLED.h>
#include <stdio.h>


WiFiClient net;
MQTTClient client(1024);


unsigned long lastMillis = 0;

#define NUM_LEDS    40          // FastLED definitions
#define LED_PIN     5
int iCooling = 40;
int iSparking = 300;
int iSparks = 20;
int iSparkHeight = 12;
bool bReversed = true;
bool bMirrored = false;

bool bChanged = 0;

CRGB g_LEDs[NUM_LEDS] = {0};    // Frame buffer for FastLED

int g_lineHeight = 0;
int g_Brightness = 255;         // 0-255 LED brightness scale
int g_PowerLimit = 3000;         // 900mW Power Limit

#include "ledgfx.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"

ClassicFireEffect *fire = nullptr;

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wifi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_server, mqtt_user, mqtt_pass))
    {
      Serial.println("connected");
      client.subscribe("esp32/flashtype");
    }
    else
    {
      Serial.print("failed, rc= ");
      Serial.print(client.lastError());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void messageReceived(MQTTClient *client, char topic[], char message[], int length)
  {
  Serial.print("Incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(message);

  if (String(topic) == "esp32/flashtype")
  {
    char delim[] = ";";
    char iVariable[15];
    int iValue = 0;

    //fire(NUM_LEDS, iCooling, iSparking, iSparks, iSparkHeight, true, false);
    char *ptr = strtok(message, delim);

    Serial.print("message: ");
    Serial.println(ptr);
  

    while(ptr != NULL) {
      int ret = sscanf(ptr, "%s %d", iVariable, &iValue);

      ptr = strtok(NULL, delim);
      if (strncmp(iVariable, "iCo", 3) == 0) {
        iCooling = iValue;
      }
      if (strncmp(iVariable, "iSparki", 7) == 0) {
        iSparking = iValue;
      }
      if (strncmp(iVariable, "iSparks", 7) == 0) {
        iSparks = iValue;
      }
      if (strncmp(iVariable, "iSparkH", 7) == 0) {
        iSparkHeight = iValue;
      }
      if (strncmp(iVariable, "bRev", 4) == 0) {
        bReversed = iValue;
      }
      if (strncmp(iVariable, "bMi", 3) == 0) {
        bMirrored = iValue;
      }
      Serial.print("iVariable = ");
      Serial.println(iVariable);
      Serial.print("iValue = ");
      Serial.println(iValue);

    }

    bChanged = 1;
  }

}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) { }
  Serial.println("ESP32 Startup");

  setup_wifi();

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  client.begin(mqtt_server, 1883, net);
  client.onMessageAdvanced(messageReceived);

  reconnect();

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS);               // Add our LED strip to the FastLED library
  FastLED.setBrightness(g_Brightness);
  set_max_power_indicator_LED(LED_BUILTIN);                               // Light the builtin LED if we power throttle
  FastLED.setMaxPowerInMilliWatts(g_PowerLimit);                          // Set the power limit, above which brightness will be throttled

//  ClassicFireEffect fire(NUM_LEDS, iCooling, iSparking, iSparks, iSparkHeight, bReversed, bMirrored);     // More Intense, Extra Sparking
  Serial.println("Before starting Fire");
//  *fire = ClassicFireEffect(NUM_LEDS, iCooling, iSparking, iSparks, iSparkHeight, bReversed, bMirrored);     // More Intense, Extra Sparking
//  ClassicFireEffect fire(NUM_LEDS, iCooling, iSparking, iSparks, iSparkHeight, bReversed, bMirrored);     // More Intense, Extra Sparking
  Serial.println("After starting Fire");
}

void DrawMarqueeComparison()
{
  static float scroll = 0.0f;
  scroll += 0.1f;
  if (scroll > 5.0)
    scroll -= 5.0;

  for (float i = scroll; i < NUM_LEDS/2 -1; i+= 5)
  {
    DrawPixels(i, 3, CRGB::Green);
    DrawPixels(NUM_LEDS-1-(int)i, 3, CRGB::Red);
  }
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability


  if (millis() - lastMillis > 1000) {
    if (!client.connected()) {
      Serial.print("lastError: ");
      Serial.println(client.lastError());
      reconnect();
    }
    lastMillis = millis();
  }

//  bool bLED = 0;

  if (bChanged == 1)
    {
      Serial.println("Inside bChanged");
      if (fire != nullptr)
        delete fire;
        *fire = ClassicFireEffect(NUM_LEDS, iCooling, iSparking, iSparks, iSparkHeight, bReversed, bMirrored);
      bChanged = 0;
  
    }

  FastLED.clear();
  Serial.println("After Fastled.clear");
  fire->DrawFire();
  Serial.println("After DrawFire");
  FastLED.show(g_Brightness);                          //  Show and delay
  Serial.println("After Fastled.show");

  delay(33);

}

