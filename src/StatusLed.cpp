// SPDX-License-Identifier: AGPL-3.0-or-later
#include "StatusLed.h"

void StatusLed::begin(const AppConfig &cfg) {
  _pin = cfg.statusLedPin;
  _activeLow = cfg.statusLedActiveLow;
  if (_pin < 0) return;
  pinMode(_pin, OUTPUT);
  _init = true;
  _on = false;
  write(false);
}

void StatusLed::write(bool on) {
  if (!_init) return;
  digitalWrite(_pin, (_activeLow ? !on : on) ? HIGH : LOW);
}

void StatusLed::loop() {
  if (!_init) return;
  uint32_t now = millis();
  bool desired;
  switch (_mode) {
  case Mode::Online:
    desired = !(now < _blipUntil); // solid on, flick off during activity
    break;
  case Mode::Portal:
    desired = ((now / 250) & 1) == 0; // ~2 Hz
    break;
  case Mode::Boot:
  default:
    desired = ((now / 100) & 1) == 0; // ~5 Hz
    break;
  }
  if (desired != _on) {
    _on = desired;
    write(_on);
  }
}