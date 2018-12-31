#!/bin/bash
rm -rf dist
rm -rf data
# ui5 build self-contained
# ui5 build --all --exclude-task=createDebugFiles
ui5 build preload --exclude-task=createDebugFiles
# rpl "/resources/sap-ui-core.js" "/res/sap-ui-custom.js" dist/index.htm
# mv dist/resources dist/res
#rm -r dist/view
#rm -r dist/resources
#rm -r dist/controller
#rm -r dist/extended*
cp -r dist data
# grunt compress
/Users/gwolf/.platformio/packages/tool-mkspiffs/mkspiffs_espressif8266_arduino --size 0xFB000 --page 256 --block 4096 -d 5 --create data/ IASServoRemote.spiffs.bin
# /Users/gwolf/.platformio/packages/tool-mkspiffs/mkspiffs_espressif8266_arduino --size 1004 --page 256 --block 8192 -d 5 --create data/ IASServoRemote.spiffs.bin

