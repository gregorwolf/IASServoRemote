#!/bin/bash
rm -rf data
npm run-script build
# ui5 build --all --exclude-task=createDebugFiles
# ui5 build preload --exclude-task=createDebugFiles
mv dist/index-multiple-parts.htm dist/index.htm
mv dist/resources dist/res
grunt compress
rm -r data/res/sap
rm -r data/**/*.js
rm -r data/**/*-dbg.*
rm -r data/controller
rm -r data/extended*
rm data/res/jquery*
rm data/res/*boot*
rm data/res/*core*
rm data/res/*debug*
# /Users/gwolf/.platformio/packages/tool-mkspiffs/mkspiffs_espressif8266_arduino --size 0xFB000 --page 256 --block 4096 -d 5 --create data/ IASServoRemote.spiffs.bin
# /Users/gwolf/.platformio/packages/tool-mkspiffs/mkspiffs_espressif8266_arduino --size 1004 --page 256 --block 8192 -d 5 --create data/ IASServoRemote.spiffs.bin
