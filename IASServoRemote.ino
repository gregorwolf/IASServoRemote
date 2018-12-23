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

IOTAppStory IAS(COMPDATE, MODEBUTTON);  // Initialize IotAppStory
AsyncWebServer server(80);              // Initialize AsyncWebServer
Servo myservo;                          // create servo object to control a servo

//called when the url is not defined here return 404
void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

// ================================================ VARS =================================================
int pos;
bool pressed;
bool toDetach;
unsigned long printEntry;
unsigned long pressTimestamp;
String deviceName = "tueroeffner";
String chipId;

// We want to be able to edit these example variables below from the wifi config manager
// Currently only char arrays are supported.
// Use functions like atoi() and atof() to transform the char array to integers or floats
// Use IAS.dPinConv() to convert Dpin numbers to integers (D6 > 14)

char* anglePress   = "75";
char* angleRelease = "140";
char* pressTime    = "1000"; // ms

char* servoPin = "16";
char* ledPin = "2";

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

  IAS.addField(pressTime,    "Press Time (ms)", 8, 'N');         // These fields are added to the "App Settings" page in config mode and saved to eeprom. Updated values are returned to the original variable.
  IAS.addField(anglePress,   "Angle Press",     8, 'N');         // reference to org variable | field label value | max char return | Optional "special field" char
  IAS.addField(angleRelease, "Angle Release",   8, 'N');         // Find out more about the optional "special fields" at https://iotappstory.com/wiki
  
  IAS.addField(ledPin,   "LED Pin",   2, 'P');
  IAS.addField(servoPin, "Servo Pin", 2, 'P');

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
  
  /* 
  IAS.onModeButtonNoPress([]() {
    Serial.println(F(" Mode Button is not pressed."));
    Serial.println(F("*-------------------------------------------------------------------------*"));
  });
  
  IAS.onFirstBoot([]() {                              
    Serial.println(F(" Run or display something on the first time this app boots"));
    Serial.println(F("*-------------------------------------------------------------------------*"));
  });

  IAS.onFirmwareUpdateCheck([]() {
    Serial.println(F(" Checking if there is a firmware update available."));
    Serial.println(F("*-------------------------------------------------------------------------*"));
  });

  IAS.onFirmwareUpdateDownload([]() {
    Serial.println(F(" Downloading and Installing firmware update."));
    Serial.println(F("*-------------------------------------------------------------------------*"));
  });

  IAS.onFirmwareUpdateError([]() {
    Serial.println(F(" Update failed...Check your logs"));
    Serial.println(F("*-------------------------------------------------------------------------*"));
  });

  IAS.onConfigMode([]() {
    Serial.println(F(" Starting configuration mode. Search for my WiFi and connect to 192.168.4.1."));
    Serial.println(F("*-------------------------------------------------------------------------*"));
  });
  */

	/* TIP! delete the lines above when not used */
 
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
  
  Serial.print(F(" Attach Servo to Pin: "));
  Serial.println(servoPin);
  myservo.attach(atoi(servoPin));
  pos = atoi(angleRelease);
  Serial.print(F(" Move Servo to: "));
  Serial.println(pos);
  myservo.write(pos);
  

  // When the button is pressed in the WebApp     <<<<<<<<<<<<--------------- <<<-------------------- <<<-----------
  server.on("/btn", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println(F("\n WebApp button pressed"));

    myservo.attach(atoi(servoPin));
    pressTimestamp = millis();
    pos = atoi(anglePress);
    Serial.print(F(" pressTimestamp "));
    Serial.println(pressTimestamp);
    Serial.print(F(" Move Servo to: "));
    Serial.println(pos);
    pressed = true;
    myservo.write(pos);

    // create json return
    String json = "{";
    json += "\"pos\":\""+String(pos)+"\"";
    json += "}";

    // return json to WebApp
    request->send(200, F("text/json"), json);
    json = String();
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
  
  if (millis() - pressTimestamp > atol(pressTime) && pressed) {
    myservo.attach(atoi(servoPin));
    pos = atoi(angleRelease);
    pressed = false;
    toDetach = true;
    Serial.print(F(" Move Servo to: "));
    Serial.println(pos);
    myservo.write(pos);
  }

  if (millis() - pressTimestamp > atol(pressTime) * 2 && toDetach) {
    myservo.detach(); 
    toDetach = false;
  }
}
