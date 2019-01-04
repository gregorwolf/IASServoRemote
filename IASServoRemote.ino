/*
	This is an initial sketch to be used as a "blueprint" to create apps which can be used with IOTappstory.com infrastructure
  Your code can be filled wherever it is marked.

  Copyright (c) [2016] [Andreas Spiess]

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  virginSoilFull V2.2.2
*/

#define COMPDATE __DATE__ __TIME__
#define MODEBUTTON 0                                        // Button pin on the esp for selecting modes. D3 for the Wemos!

#include <IOTAppStory.h>                                    // IotAppStory.com library

#if defined ESP8266
  #include <ESPAsyncTCP.h>                            // https://github.com/me-no-dev/ESPAsyncTCP
#elif defined ESP32
  #include <AsyncTCP.h>                               // https://github.com/me-no-dev/AsyncTCP
  #include <SPIFFS.h>
#endif

#include <ESPAsyncWebServer.h>                        // https://github.com/me-no-dev/ESPAsyncWebServer
#include <FS.h>

#include <Servo.h>
#include <ArduinoJson.h>

IOTAppStory IAS(COMPDATE, MODEBUTTON);  // Initialize IotAppStory
AsyncWebServer server(80);              // Initialize AsyncWebServer
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
Servo myservo1;                          // create servo object to control a servo
Servo myservo2;                          // create servo object to control a servo

// ================================================ VARS =================================================
int pos;
bool pressed;
bool toDetach;
unsigned long printEntry;
unsigned long pressTimestamp;
String deviceName = "servoremote";
String chipId;

// We want to be able to edit these example variables below from the wifi config manager
// Currently only char arrays are supported.
// Use functions like atoi() and atof() to transform the char array to integers or floats
// Use IAS.dPinConv() to convert Dpin numbers to integers (D6 > 14)

char* angleMin    = "0";
char* angleCenter = "90";
char* angleMax    = "180";

char* servo1Pin = "16"; // D0
char* servo2Pin = "14"; // D5
char* ledPin = "2";

const size_t bufferSize = JSON_OBJECT_SIZE(2) + 20;
DynamicJsonBuffer jsonBuffer(bufferSize);

// ================================================ SETUP ================================================
void setup() {
  #if defined  ESP8266
    chipId  = String(ESP.getChipId());   // creat a unique deviceName for classroom situations (deviceName-123)
    Serial.print(F(" chipId: "));
    Serial.println(chipId);
    String shortChipId         = "-"+chipId.substring(chipId.length()-3);
    deviceName    += shortChipId;
  #endif  
  /* TIP! delete lines below when not used */
  IAS.preSetDeviceName(deviceName);                   // preset deviceName this is also your MDNS responder: http://websocketservo.local
  IAS.preSetAutoUpdate(true);                         // automaticUpdate (true, false)
  //IAS.preSetAutoConfig(false);                      // automaticConfig (true, false)
  //IAS.preSetWifi("SSID","PASSWORD");                // preset Wifi
  /* TIP! Delete Wifi cred. when you publish your App. */

  IAS.addField(angleMin,    "Angle Min",     8, 'N');
  IAS.addField(angleCenter, "Angle Center",     8, 'N');
  IAS.addField(angleMax,    "Angle Max",   8, 'N');
  
  IAS.addField(ledPin,   "LED Pin",   2, 'P');
  IAS.addField(servo1Pin, "Servo 1 Pin", 2, 'P');
  IAS.addField(servo2Pin, "Servo 2 Pin", 2, 'P');

  // You can configure callback functions that can give feedback to the app user about the current state of the application.
  // In this example we use serial print to demonstrate the call backs. But you could use leds etc.

  IAS.onModeButtonShortPress([]() {
    Serial.println(F(" If mode button is released, I will enter in firmware update mode."));
    Serial.println(F("*-------------------------------------------------------------------------*"));
  });

  IAS.onModeButtonLongPress([]() {
    Serial.println(F(" If mode button is released, I will enter in configuration mode."));
    Serial.println(F("*-------------------------------------------------------------------------*"));
  });

  IAS.onModeButtonVeryLongPress([]() {
    Serial.println(F(" If mode button is released, I won't do anything unless you program me to."));
    Serial.println(F("*-------------------------------------------------------------------------*"));
    /* TIP! You can use this callback to put your app on it's own configuration mode */
  });
 
  //IAS.begin();
  IAS.begin('P');                          // Optional parameter: What to do with EEPROM on First boot of the app? 'F' Fully erase | 'P' Partial erase(default) | 'L' Leave intact

  IAS.setCallHome(true);                   // Set to true to enable calling home frequently (disabled by default)
  IAS.setCallHomeInterval(14400);           // Call home interval in seconds, use 60s only for development. Please change it to at least 2 hours in production


  //-------- Your Setup starts from here ---------------
  Serial.println(F("*-------------------------------------------------------------------------*"));
  if(!SPIFFS.begin()){
      Serial.println(F(" SPIFFS Mount Failed"));
      return;
  }
  
  Serial.print(F(" Attach Servo 1 to Pin: "));
  Serial.println(servo1Pin);
  myservo1.attach(atoi(servo1Pin));
  Serial.print(F(" Attach Servo 2 to Pin: "));
  Serial.println(servo2Pin);
  myservo2.attach(atoi(servo2Pin));
  pos = atoi(angleCenter);
  Serial.print(F(" Move Servo 1 and 2 to: "));
  Serial.println(pos);
  myservo1.write(pos);
  myservo2.write(pos);  

  // attach AsyncWebSocket
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  
  // When the button is pressed in the WebApp
  server.on("/min", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println(F("\n Min button pressed"));
    pos = atoi(angleMin);
    request->send(200, F("text/json"), moveServoTo(myservo1, pos));
  }); 

  server.on("/max", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println(F("\n Min button pressed"));
    pos = atoi(angleMax);
    request->send(200, F("text/json"), moveServoTo(myservo1, pos));
  });

  server.on("/getState", HTTP_GET, [](AsyncWebServerRequest *request){
    // create json return
    String json = "{";
    json += "\"pos\":\""+String(pos)+"\"";
    json += "}";

    // return json to WebApp
    request->send(200, F("text/json"), json);
    json = String();
  });

  server.serveStatic("/", SPIFFS, "/");

  server.onNotFound(onRequest);

  // start the HTTP server
  server.begin();
  Serial.println(F(" HTTP server started"));
  Serial.println(F("*-------------------------------------------------------------------------*"));
}

// ================================================ LOOP =================================================
void loop() {
  IAS.loop(); // this routine handles the calling home functionality and reaction of the MODEBUTTON pin. 
              // If short press (<4 sec): update of sketch, long press (>7 sec): Configuration

  //-------- Your Sketch starts from here ---------------
  
}

// ================================================ Extra functions ======================================

//called when the url is not defined here return 404
void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

String moveServoTo(Servo servo, int pos){
    // Serial.print(F(" Move Servo to: "));
    // Serial.println(pos);
    servo.write(pos);

    // create json return
    String json = "{";
    json += "\"pos\":\""+String(pos)+"\"";
    json += "}";
    return json;
}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //client connected
    os_printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    //client disconnected
    os_printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    //error was received from the other end
    os_printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    //pong message was received (in response to a ping request maybe)
    os_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    //data packet
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      os_printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
      if(info->opcode == WS_TEXT){
        data[len] = 0;
        os_printf("%s\n", (char*)data);
      } else {
        for(size_t i=0; i < info->len; i++){
          os_printf("%02x ", data[i]);
        }
        os_printf("\n");
      }
      if(info->opcode == WS_TEXT) {
        JsonObject& root = jsonBuffer.parseObject((char*)data);
        int s = root["s"]; // 1
        int p = root["p"]; // 180
        if(s = 1) {
          moveServoTo(myservo1, p);
        } else if(s = 2) {
          moveServoTo(myservo2, p);
        }
        client->text((char*)data);
      } else {
        client->binary("I got your binary message");
      }
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          os_printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        os_printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      os_printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
      if(info->message_opcode == WS_TEXT){
        data[len] = 0;
        os_printf("%s\n", (char*)data);
      } else {
        for(size_t i=0; i < len; i++){
          os_printf("%02x ", data[i]);
        }
        os_printf("\n");
      }

      if((info->index + len) == info->len){
        os_printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          os_printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}
