// SPDX-License-Identifier: AGPL-3.0-or-later
// CaptivePortal — SoftAP + DNS wildcard so any hostname resolves to the device,
// used for first-time / offline configuration. The web server serves the same
// UI; this just provides AP + DNS and OS captive-portal detection redirects.
#pragma once

#include <Arduino.h>
#include <DNSServer.h>
#include "AppConfig.h"

class CaptivePortal {
public:
  void begin(const AppConfig &cfg);
  void loop(); // must be pumped to answer DNS queries
  bool active() const { return _active; }
  IPAddress apIP() const { return _apIP; }

private:
  DNSServer _dns;
  IPAddress _apIP{192, 168, 4, 1};
  bool _active = false;
};