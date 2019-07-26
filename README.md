<img src="/tools/logo/TASMOTA_FullLogo_Vector.svg" alt="Logo" align="right" height="76"/>

# Sonoff-Tasmota
I have tried integrating [McLighting](https://github.com/toblum/McLighting) which uses [WS2812fx](https://github.com/kitesurfer1404/WS2812FX) to provide 50+ effects. If anyone is interested, you can clone and test my fork here. Change `my_led_config.h` file accordingly. To control the effects using MQTT, send payload to topic  `cmnd/sonoff`. To see results, subscribe to `stat/sonoff/LED`. Checkout this [API](https://github.com/toblum/McLighting/wiki/WebSocket-API) for how to send payload. Don't know if it messes up other sonoff code. Hope this helps someone!
