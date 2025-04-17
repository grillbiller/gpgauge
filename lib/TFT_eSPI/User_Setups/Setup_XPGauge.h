
#define GC9A01_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#define TFT_MOSI 23
#define TFT_SCLK 18

// These are not fixed; will be set dynamically with pinMode/digitalWrite per screen
// Only one screen active at a time using different CS pin per display

#define TFT_DC   2
#define TFT_RST  4

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT
