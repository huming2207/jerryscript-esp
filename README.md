# JerryScript for ESP32

WIP, just a toy for nerds.

Source is from here with some tweaks: [Single source build mode](https://github.com/jerryscript-project/jerryscript/blob/master/docs/01.CONFIGURATION.md#single-source-build-mode)

Currently I'm using `v2.3.0` instead of `master` branch

## To-do 

- ESP-IDF `menuconifg` stuff for configuring Jerry's heap size etc. 
- For now, configurations are hard-coded, for ESP32-WROVER modules (with PSRAM)
    - Heap size is set to 3072KB
    - Using system allocator instead of static allocator
- More testings are needed
