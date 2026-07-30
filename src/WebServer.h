// SPDX-License-Identifier: AGPL-3.0-or-later
// WebServer — serves the embedded SPA and the JSON control/config API. Exposes
// the underlying AsyncWebServer so OTA can mount onto the same instance.
#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "AppConfig.h"
#include "ViscaSerial.h"
#include "NetLink.h"
#include "ViscaProxy.h"

class WebServer {
public:
  WebServer() : _server(80) {}

  void begin(AppConfig &cfg, ViscaSerial &serial, NetLink &net,
             ViscaProxy &proxy, bool captive);

  AsyncWebServer &raw() { return _server; }

private:
  void registerAssets();
  void registerApi();
  ViscaFrame runCommand(const ViscaFrame &frame, bool wantReply);

  AsyncWebServer _server;
  AppConfig *_cfg = nullptr;
  ViscaSerial *_serial = nullptr;
  NetLink *_net = nullptr;
  ViscaProxy *_proxy = nullptr;
  bool _captive = false;
};