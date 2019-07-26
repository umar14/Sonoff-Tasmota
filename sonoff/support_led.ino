// ***************************************************************************
// Request handlers
// ***************************************************************************

void initStrip()
{
  strip->init();
  strip->setBrightness(brightness);
  strip->setSpeed(convertSpeed(ws2812fx_speed));
  //strip->setMode(ws2812fx_mode);
  strip->setColor(main_color.red, main_color.green, main_color.blue);
  strip->setOptions(0, GAMMA); // We only have one WS2812FX segment, set color to human readable GAMMA correction
  strip->start();
  if (mode != HOLD)
    mode = SET_MODE;
}

uint16_t convertSpeed(uint8_t mcl_speed)
{
  //long ws2812_speed = mcl_speed * 256;
  uint16_t ws2812_speed = 61760 * (exp(0.0002336 * mcl_speed) - exp(-0.03181 * mcl_speed));
  ws2812_speed = SPEED_MAX - ws2812_speed;
  if (ws2812_speed < SPEED_MIN)
  {
    ws2812_speed = SPEED_MIN;
  }
  if (ws2812_speed > SPEED_MAX)
  {
    ws2812_speed = SPEED_MAX;
  }
  return ws2812_speed;
}

// ***************************************************************************
// Handler functions for MQTT
// ***************************************************************************

void handleSetSingleLED(uint8_t *mypayload, uint8_t firstChar) //= 0)
{
  // decode led index
  char templed[3];
  strncpy(templed, (const char *)&mypayload[firstChar], 2);
  uint8_t led = atoi(templed);

  // DBG_OUTPUT_PORT.printf("LED: led value: [%i]. Entry threshold: <= [%i] (=> %s)\n", led, strip->numPixels(), mypayload);
  if (led <= strip->numPixels())
  {
    char redhex[3];
    char greenhex[3];
    char bluehex[3];
    strncpy(redhex, (const char *)&mypayload[2 + firstChar], 2);
    strncpy(greenhex, (const char *)&mypayload[4 + firstChar], 2);
    strncpy(bluehex, (const char *)&mypayload[6 + firstChar], 2);
    redhex[2] = 0x00;
    greenhex[2] = 0x00;
    bluehex[2] = 0x00;
    ledstates[led].red = strtol(redhex, NULL, 16);
    ledstates[led].green = strtol(greenhex, NULL, 16);
    ledstates[led].blue = strtol(bluehex, NULL, 16);
    // DBG_OUTPUT_PORT.printf("LED: rgb.red: [%s] rgb.green: [%s] rgb.blue: [%s]\n", redhex, greenhex, bluehex);
    // DBG_OUTPUT_PORT.printf("LED: rgb.red: [%i] rgb.green: [%i] rgb.blue: [%i]\n", strtol(redhex, NULL, 16), strtol(greenhex, NULL, 16), strtol(bluehex, NULL, 16));
    // DBG_OUTPUT_PORT.printf("LED: Set single led [%i] to [%i] [%i] [%i] (%s)!\n", led, ledstates[led].red, ledstates[led].green, ledstates[led].blue, mypayload);

    strip->setPixelColor(led, ledstates[led].red, ledstates[led].green, ledstates[led].blue);
    strip->show();
  }

  exit_func = true;
  mode = CUSTOM;
}

void handleSetMainColor(uint8_t *mypayload)
{
  // decode rgb data
  uint32_t rgb = (uint32_t)strtol((const char *)&mypayload[1], NULL, 16);
  main_color.red = ((rgb >> 16) & 0xFF);
  main_color.green = ((rgb >> 8) & 0xFF);
  main_color.blue = ((rgb >> 0) & 0xFF);
  //  strip->setColor(main_color.red, main_color.green, main_color.blue);
  mode = SETCOLOR;
}

void handleSetAllMode(uint8_t *mypayload)
{
  // decode rgb data
  uint32_t rgb = (uint32_t)strtol((const char *)&mypayload[1], NULL, 16);

  main_color.red = ((rgb >> 16) & 0xFF);
  main_color.green = ((rgb >> 8) & 0xFF);
  main_color.blue = ((rgb >> 0) & 0xFF);
  // AddLog_P2(LOG_LEVEL_INFO, PSTR("LED: Set main color and light all LEDs [%s]"), "x");
  // DBG_OUTPUT_PORT.printf("LED: Set all leds to main color: [%u] [%u] [%u]\n", main_color.red, main_color.green, main_color.blue);
  exit_func = true;
  ws2812fx_mode = FX_MODE_STATIC;
  mode = SET_MODE;
}

void handleSetDifferentColors(uint8_t *mypayload)
{
  uint8_t *nextCommand = 0;
  nextCommand = (uint8_t *)strtok((char *)mypayload, "+");
  while (nextCommand)
  {
    handleSetSingleLED(nextCommand, 0);
    nextCommand = (uint8_t *)strtok(NULL, "+");
  }
}

void handleRangeDifferentColors(uint8_t *mypayload)
{
  uint8_t *nextCommand = 0;
  nextCommand = (uint8_t *)strtok((char *)mypayload, "R");
  // While there is a range to process R0110<00ff00>

  while (nextCommand)
  {
    // Loop for each LED.
    char startled[3] = {0, 0, 0};
    char endled[3] = {0, 0, 0};
    char colorval[7] = {0, 0, 0, 0, 0, 0, 0};
    strncpy(startled, (const char *)&nextCommand[0], 2);
    strncpy(endled, (const char *)&nextCommand[2], 2);
    strncpy(colorval, (const char *)&nextCommand[4], 6);
    int rangebegin = atoi(startled);
    int rangeend = atoi(endled);
    // DBG_OUTPUT_PORT.printf("LED: Setting RANGE from [%i] to [%i] as color [%s]\n", rangebegin, rangeend, colorval);

    while (rangebegin <= rangeend)
    {
      char rangeData[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
      if (rangebegin < 10)
      {
        // Create the valid 'nextCommand' structure
        sprintf(rangeData, "0%d%s", rangebegin, colorval);
      }
      if (rangebegin >= 10)
      {
        // Create the valid 'nextCommand' structure
        sprintf(rangeData, "%d%s", rangebegin, colorval);
      }
      // Set one LED
      handleSetSingleLED((uint8_t *)rangeData, 0);
      rangebegin++;
    }

    // Next Range at R
    nextCommand = (uint8_t *)strtok(NULL, "R");
  }
}

void handleSetNamedMode(String str_mode)
{
  exit_func = true;

  if (str_mode.startsWith("=off"))
  {
    mode = OFF;
  }
  if (str_mode.startsWith("=all"))
  {
    ws2812fx_mode = FX_MODE_STATIC;
    mode = SET_MODE;
  }
  if (str_mode.startsWith("=wipe"))
  {
    mode = WIPE;
  }
  if (str_mode.startsWith("=rainbow"))
  {
    mode = RAINBOW;
  }
  if (str_mode.startsWith("=rainbowCycle"))
  {
    mode = RAINBOWCYCLE;
  }
  if (str_mode.startsWith("=theaterchase"))
  {
    mode = THEATERCHASE;
  }
  if (str_mode.startsWith("=twinkleRandom"))
  {
    mode = TWINKLERANDOM;
  }
  if (str_mode.startsWith("=theaterchaseRainbow"))
  {
    mode = THEATERCHASERAINBOW;
  }
}

void handleSetWS2812FXMode(uint8_t *mypayload)
{
  mode = SET_MODE;
  uint8_t ws2812fx_mode_tmp = (uint8_t)strtol((const char *)&mypayload[1], NULL, 10);
  ws2812fx_mode = constrain(ws2812fx_mode_tmp, 0, strip->getModeCount() - 1);
}

String listStatusJSON(void)
{
  uint8_t tmp_mode = (mode == SET_MODE) ? (uint8_t)ws2812fx_mode : strip->getMode();

  StaticJsonBuffer<512> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["mode"] = (uint8_t)mode;
  root["ws2812fx_mode"] = tmp_mode;
  root["ws2812fx_mode_name"] = strip->getModeName(tmp_mode);
  root["speed"] = ws2812fx_speed;
  root["brightness"] = brightness;
  JsonArray &color = root.createNestedArray("color");
  color.add(main_color.red);
  color.add(main_color.green);
  color.add(main_color.blue);

  String json;
  root.printTo(json);

  return json;
}
