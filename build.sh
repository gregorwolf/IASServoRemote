#!/bin/bash
npm install
rm -rf data
npm run-script build
#mv dist/index-multiple-parts.htm dist/index.htm
mv dist/resources dist/res
grunt compress
rm -r data/res/sap
rm -r data/controller
rm data/res/jquery*
rm data/res/*boot*
rm data/res/*core*
rm data/res/*debug*
# cp dist/res/jquery.sap.script.js data/res/
# /Users/gwolf/.platformio/packages/tool-mkspiffs/mkspiffs_espressif8266_arduino --size 0xFB000 --page 256 --block 4096 -d 5 --create data/ IASServoRemote.spiffs.bin
# /Users/gwolf/.platformio/packages/tool-mkspiffs/mkspiffs_espressif8266_arduino --size 1004 --page 256 --block 8192 -d 5 --create data/ IASServoRemote.spiffs.bin
