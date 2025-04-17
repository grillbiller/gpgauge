#pragma once

#define GC9A01_DRIVER

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

#define TFT_MOSI 17
#define TFT_SCLK 23
#define TFT_CS   13  // Vi bruger én ad gangen og styrer CS manuelt i koden
#define TFT_DC   19
#define TFT_RST  18

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SPI_FREQUENCY  27000000
