// SPDX-License-Identifier: AGPL-3.0-or-later
#include "AppConfig.h"
#include <Preferences.h>

AppConfig g_config;

static const char *NVS_NAMESPACE = "visca";

String AppConfig::deviceId() {
  uint64_t mac = ESP.getEfuseMac();
  char buf[7];
  snprintf(buf, sizeof(buf), "%02x%02x%02x", (uint8_t)(mac >> 16),
           (uint8_t)(mac >> 8), (uint8_t)mac);
  return String(buf);
}

bool AppConfig::load() {
  Preferences p;
  p.begin(NVS_NAMESPACE, true); // read-only
  bool exists = p.isKey("hostname");

  // Device-unique defaults so multiple fresh units don't collide on the network.
  String did = deviceId();
  hostname = p.getString("hostname", String(DEFAULT_HOSTNAME) + "-" + did);
  wifiSsid = p.getString("wifiSsid", wifiSsid);
  wifiPass = p.getString("wifiPass", wifiPass);
  ethEnabled = p.getBool("ethEnabled", ethEnabled);

  ethSck = p.getChar("ethSck", ethSck);
  ethMiso = p.getChar("ethMiso", ethMiso);
  ethMosi = p.getChar("ethMosi", ethMosi);
  ethCs = p.getChar("ethCs", ethCs);
  ethInt = p.getChar("ethInt", ethInt);
  ethRst = p.getChar("ethRst", ethRst);

  uartTx = p.getChar("uartTx", uartTx);
  uartRx = p.getChar("uartRx", uartRx);
  serialBaud = p.getULong("serialBaud", serialBaud);

  viscaPort = p.getUShort("viscaPort", viscaPort);
  viscaAddress = p.getUChar("viscaAddr", viscaAddress);

  apSsid = p.getString("apSsid", String(DEFAULT_AP_SSID) + "-" + did);
  apPass = p.getString("apPass", apPass);
  otaPassword = p.getString("otaPass", otaPassword);

  statusLedPin = p.getShort("ledPin", statusLedPin);
  statusLedActiveLow = p.getBool("ledInv", statusLedActiveLow);

  p.end();
  return exists;
}

void AppConfig::save() const {
  Preferences p;
  p.begin(NVS_NAMESPACE, false); // read-write

  p.putString("hostname", hostname);
  p.putString("wifiSsid", wifiSsid);
  p.putString("wifiPass", wifiPass);
  p.putBool("ethEnabled", ethEnabled);

  p.putChar("ethSck", ethSck);
  p.putChar("ethMiso", ethMiso);
  p.putChar("ethMosi", ethMosi);
  p.putChar("ethCs", ethCs);
  p.putChar("ethInt", ethInt);
  p.putChar("ethRst", ethRst);

  p.putChar("uartTx", uartTx);
  p.putChar("uartRx", uartRx);
  p.putULong("serialBaud", serialBaud);

  p.putUShort("viscaPort", viscaPort);
  p.putUChar("viscaAddr", viscaAddress);

  p.putString("apSsid", apSsid);
  p.putString("apPass", apPass);
  p.putString("otaPass", otaPassword);

  p.putShort("ledPin", statusLedPin);
  p.putBool("ledInv", statusLedActiveLow);

  p.end();
}

void AppConfig::reset() {
  Preferences p;
  p.begin(NVS_NAMESPACE, false);
  p.clear();
  p.end();
}