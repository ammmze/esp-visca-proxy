// StatusLed — non-blocking onboard-LED indicator.
//   Boot/connecting: fast blink · Captive portal: slow blink · Online: solid,
//   with a brief flicker on VISCA activity.
// Uses plain pinMode/digitalWrite, which the ESP32 core transparently maps to
// the addressable RGB LED on devkits that only have one (S3/C3/S2 devkitc).
// Pin < 0 disables the indicator entirely.
#pragma once

#include <Arduino.h>
#include "AppConfig.h"

class StatusLed {
public:
  enum class Mode { Boot, Portal, Online };

  void begin(const AppConfig &cfg);
  void setMode(Mode m) { _mode = m; }
  void blip() { _blipUntil = millis() + 60; } // brief activity flash
  void loop();

private:
  void write(bool on);

  int16_t _pin = -1;
  bool _activeLow = false;
  Mode _mode = Mode::Boot;
  bool _on = false;
  bool _init = false;
  uint32_t _blipUntil = 0;
};
