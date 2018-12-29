#!/bin/bash
rm -rf dist
rm -rf data
ui5 build self-contained
rpl "/resources/" "/res/" dist/index.htm
mv dist/resources dist/res
grunt compress
/Users/gwolf/.platformio/packages/tool-mkspiffs/mkspiffs_espressif8266_arduino --size 0xFB000 --page 256 --block 4096 -d 5 --create data/ IASServoRemote.spiffs.bin
