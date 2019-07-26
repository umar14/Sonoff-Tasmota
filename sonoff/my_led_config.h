// ***************************************************************************
// Global variables / definitions
// ***************************************************************************

#define LED_PIN 14       // LED_PIN (14 / D5) where neopixel / WS2811 strip is attached
#define NUMLEDS 5       // Number of leds in the strip

WS2812FX* strip = new WS2812FX(NUMLEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
#define DBG_OUTPUT_PORT Serial  // Set debug output port

// List of all color modes
enum MODE { SET_MODE, HOLD, OFF, SETCOLOR, SETSPEED, BRIGHTNESS, WIPE, RAINBOW, RAINBOWCYCLE, THEATERCHASE, TWINKLERANDOM, THEATERCHASERAINBOW, TV, CUSTOM };
MODE mode = RAINBOW;         // Standard mode that is active when software starts
bool exit_func = false;      // Global helper variable to get out of the color modes when mode changes
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

typedef struct ledstate LEDState;     // Define the datatype LEDState
LEDState ledstates[NUMLEDS];          // Get an array of led states to store the state of the whole strip
LEDState main_color = { 255, 0, 0 };  // Store the "main color" of the strip used in single color modes

