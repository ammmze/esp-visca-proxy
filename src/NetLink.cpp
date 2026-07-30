// SPDX-License-Identifier: AGPL-3.0-or-later
#include "NetLink.h"
#include <WiFi.h>
#include <Network.h>
#include <ETH.h>
#include "config.h"

static volatile bool s_ethGotIP = false;

NetState NetLink::connect(const AppConfig &cfg) {
  WiFi.setHostname(cfg.hostname.c_str());

  if (cfg.ethEnabled && tryEthernet(cfg)) {
    _state = NetState::Ethernet;
    return _state;
  }
  if (cfg.hasWifiCreds() && tryWifi(cfg)) {
    _state = NetState::WiFi;
    return _state;
  }
  _state = NetState::Disconnected;
  return _state;
}

bool NetLink::tryEthernet(const AppConfig &cfg) {
  s_ethGotIP = false;
  Network.onEvent([](arduino_event_id_t event) {
    if (event == ARDUINO_EVENT_ETH_GOT_IP) s_ethGotIP = true;
  });

  // W5500 over SPI (SPI2_HOST). phy_addr 1 is standard for the W5500.
  bool started = ETH.begin(ETH_PHY_W5500, 1, cfg.ethCs, cfg.ethInt, cfg.ethRst,
                           SPI2_HOST, cfg.ethSck, cfg.ethMiso, cfg.ethMosi);
  if (!started) {
    log_w("ETH.begin failed (no W5500?)");
    return false;
  }

  uint32_t start = millis();
  while (!s_ethGotIP && millis() - start < NET_CONNECT_TIMEOUT_MS) {
    delay(50);
  }
  if (s_ethGotIP) {
    log_i("Ethernet up: %s", ETH.localIP().toString().c_str());
    return true;
  }
  log_w("Ethernet timed out");
  return false;
}

bool NetLink::tryWifi(const AppConfig &cfg) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPass.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < NET_CONNECT_TIMEOUT_MS) {
    delay(100);
  }
  if (WiFi.status() == WL_CONNECTED) {
    log_i("WiFi up: %s", WiFi.localIP().toString().c_str());
    return true;
  }
  log_w("WiFi timed out");
  WiFi.disconnect(true);
  return false;
}

IPAddress NetLink::localIP() const {
  switch (_state) {
  case NetState::Ethernet: return ETH.localIP();
  case NetState::WiFi: return WiFi.localIP();
  default: return IPAddress(0, 0, 0, 0);
  }
}

const char *NetLink::linkName() const {
  switch (_state) {
  case NetState::Ethernet: return "ethernet";
  case NetState::WiFi: return "wifi";
  default: return "offline";
  }
}