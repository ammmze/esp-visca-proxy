#include "CaptivePortal.h"
#include <WiFi.h>

void CaptivePortal::begin(const AppConfig &cfg) {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(_apIP, _apIP, IPAddress(255, 255, 255, 0));
  const char *pass = cfg.apPass.length() ? cfg.apPass.c_str() : nullptr;
  WiFi.softAP(cfg.apSsid.c_str(), pass);

  _dns.setErrorReplyCode(DNSReplyCode::NoError);
  _dns.start(53, "*", _apIP); // wildcard -> device
  _active = true;
  log_i("Captive portal AP '%s' at %s", cfg.apSsid.c_str(),
        _apIP.toString().c_str());
}

void CaptivePortal::loop() {
  if (_active) _dns.processNextRequest();
}
