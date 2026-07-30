// OtaManager — ArduinoOTA (espota, dev pushes) + ElegantOTA (browser upload at
// /update on the shared async web server).
#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "AppConfig.h"

class OtaManager {
public:
  void begin(const AppConfig &cfg, AsyncWebServer &server);
  void loop(); // pump ArduinoOTA + ElegantOTA

private:
  bool _started = false;
};
