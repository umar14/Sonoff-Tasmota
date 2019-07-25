// Neopixel
#define LED_PIN 14       // LED_PIN (14 / D5) where neopixel / WS2811 strip is attached
#define NUMLEDS 5       // Number of leds in the strip
#define ENABLE_MQTT        // If defined, enable MQTT client code, see: https://github.com/toblum/McLighting/wiki/MQTT-API
#define ENABLE_LEGACY_ANIMATIONS // Dont disbale this for now
// #define ENABLE_STATE_SAVE_SPIFFS        // If defined, saves state on SPIFFS (USE ONLY WITH 4M FLASH)

// parameters for automatically cycling favorite patterns
uint32_t autoParams[][4] = {  // color, speed, mode, duration (milliseconds)
  {0xff0000, 200,  1,  5000}, // blink red for 5 seconds
  {0x00ff00, 200,  3, 10000}, // wipe green for 10 seconds
  {0x0000ff, 200, 14,  5000}, // dual scan blue for 5 seconds
  {0x0000ff, 200, 45, 15000}  // fireworks for 15 seconds
};

// ***************************************************************************
// Global variables / definitions
// ***************************************************************************
#define DBG_OUTPUT_PORT Serial  // Set debug output port

// List of all color modes
#ifdef ENABLE_LEGACY_ANIMATIONS
  #ifdef ENABLE_E131
    enum MODE { SET_MODE, HOLD, OFF, SETCOLOR, SETSPEED, BRIGHTNESS, WIPE, RAINBOW, RAINBOWCYCLE, THEATERCHASE, TWINKLERANDOM, THEATERCHASERAINBOW, TV, CUSTOM, E131 };
  #else
    enum MODE { SET_MODE, HOLD, OFF, SETCOLOR, SETSPEED, BRIGHTNESS, WIPE, RAINBOW, RAINBOWCYCLE, THEATERCHASE, TWINKLERANDOM, THEATERCHASERAINBOW, TV, CUSTOM };
  #endif
  MODE mode = RAINBOW;         // Standard mode that is active when software starts
  bool exit_func = false;      // Global helper variable to get out of the color modes when mode changes
#else
  enum MODE { SET_MODE, HOLD, OFF, SETCOLOR, SETSPEED, BRIGHTNESS, CUSTOM };
  MODE mode = SET_MODE;        // Standard mode that is active when software starts
#endif

MODE prevmode = mode;

int ws2812fx_speed = 196;   // Global variable for storing the delay between color changes --> smaller == faster
int brightness = 196;       // Global variable for storing the brightness (255 == 100%)

int ws2812fx_mode = 0;      // Helper variable to set WS2812FX modes

struct ledstate             // Data structure to store a state of a single led
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

struct
{
  uint16_t stripSize = NUMLEDS;
  uint8_t RGBOrder = NEO_GRB;
  uint8_t pin = LED_PIN;
} WS2812FXStripSettings;

Ticker saveWS2812FXStripSettings;

typedef struct ledstate LEDState;     // Define the datatype LEDState
LEDState ledstates[NUMLEDS];          // Get an array of led states to store the state of the whole strip
LEDState main_color = { 255, 0, 0 };  // Store the "main color" of the strip used in single color modes

#ifdef ENABLE_STATE_SAVE_SPIFFS
  bool updateStateFS = false;
#endif
