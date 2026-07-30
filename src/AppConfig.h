// SPDX-License-Identifier: AGPL-3.0-or-later
// AppConfig — persistent runtime configuration stored in NVS (Preferences).
#pragma once

#include <Arduino.h>
#include "config.h"

struct AppConfig {
  // Network
  String hostname = DEFAULT_HOSTNAME;
  String wifiSsid = "";
  String wifiPass = "";
  bool ethEnabled = false;

  // W5500 SPI pins
  int8_t ethSck = DEF_ETH_SCK;
  int8_t ethMiso = DEF_ETH_MISO;
  int8_t ethMosi = DEF_ETH_MOSI;
  int8_t ethCs = DEF_ETH_CS;
  int8_t ethInt = DEF_ETH_INT;
  int8_t ethRst = DEF_ETH_RST;

  // Serial (MAX3232)
  int8_t uartTx = DEF_UART_TX;
  int8_t uartRx = DEF_UART_RX;
  uint32_t serialBaud = DEFAULT_SERIAL_BAUD;

  // VISCA
  uint16_t viscaPort = DEFAULT_VISCA_UDP_PORT;
  uint8_t viscaAddress = DEFAULT_VISCA_ADDRESS;

  // Access point (captive portal)
  String apSsid = DEFAULT_AP_SSID;
  String apPass = DEFAULT_AP_PASSWORD;

  // OTA
  String otaPassword = "";

  // Status LED (-1 disables)
  int16_t statusLedPin = DEF_STATUS_LED;
  bool statusLedActiveLow = DEF_STATUS_LED_ACTIVE_LOW;

  // Presets: how many preset buttons to show, plus optional custom labels
  // stored as a JSON array string (e.g. ["Wide","Podium"]).
  uint8_t presetCount = DEFAULT_PRESET_COUNT;
  String presetNames = "[]";

  // Load from NVS (fills defaults on first boot). Returns true if a stored
  // config was found.
  bool load();
  // Persist current values to NVS.
  void save() const;
  // Wipe stored config back to defaults.
  static void reset();

  // Short, stable per-chip id (6 lowercase hex from the factory MAC). Used to
  // make the default hostname / AP SSID unique across devices.
  static String deviceId();

  bool hasWifiCreds() const { return wifiSsid.length() > 0; }
};

// Global instance.
extern AppConfig g_config;