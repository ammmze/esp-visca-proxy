// SPDX-License-Identifier: AGPL-3.0-or-later
#include "OtaManager.h"
#include <ArduinoOTA.h>
#include <ElegantOTA.h>

void OtaManager::begin(const AppConfig &cfg, AsyncWebServer &server) {
  // Browser upload at /update.
  ElegantOTA.begin(&server);
  if (cfg.otaPassword.length()) {
    ElegantOTA.setAuth("admin", cfg.otaPassword.c_str());
  }

  // espota / IDE pushes.
  ArduinoOTA.setHostname(cfg.hostname.c_str());
  if (cfg.otaPassword.length()) {
    ArduinoOTA.setPassword(cfg.otaPassword.c_str());
  }
  ArduinoOTA.onStart([]() { log_i("ArduinoOTA start"); });
  ArduinoOTA.onError([](ota_error_t e) { log_e("ArduinoOTA error %u", e); });
  ArduinoOTA.begin();

  _started = true;
}

void OtaManager::loop() {
  if (!_started) return;
  ArduinoOTA.handle();
  ElegantOTA.loop();
}