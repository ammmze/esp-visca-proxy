// NetLink — brings up Ethernet (W5500) and/or WiFi STA, reporting which
// link (if any) obtained connectivity. Captive-portal fallback is handled by
// the caller when this returns Disconnected.
#pragma once

#include <Arduino.h>
#include "AppConfig.h"

enum class NetState { Disconnected, Ethernet, WiFi };

class NetLink {
public:
  // Attempt Ethernet (if enabled) then WiFi STA. Returns the link that came up,
  // or Disconnected if neither did.
  NetState connect(const AppConfig &cfg);

  NetState state() const { return _state; }
  bool isOnline() const { return _state != NetState::Disconnected; }
  IPAddress localIP() const;
  const char *linkName() const;

private:
  bool tryEthernet(const AppConfig &cfg);
  bool tryWifi(const AppConfig &cfg);

  NetState _state = NetState::Disconnected;
};
