#include <Arduino.h>
#include "config.h"
#include "AppConfig.h"
#include "ViscaSerial.h"
#include "ViscaProxy.h"
#include "NetLink.h"
#include "CaptivePortal.h"
#include "WebServer.h"
#include "OtaManager.h"
#include "StatusLed.h"

static ViscaSerial serial;
static ViscaProxy proxy;
static NetLink net;
static CaptivePortal captive;
static WebServer web;
static OtaManager ota;
static StatusLed led;

void setup() {
  Serial.begin(115200);
  delay(100);
  log_i("%s %s booting", FW_NAME, FW_VERSION);

  g_config.load();
  led.begin(g_config);
  led.setMode(StatusLed::Mode::Boot);
  serial.begin(g_config);

  net.connect(g_config);

  if (!net.isOnline()) {
    // No network → captive portal for setup.
    captive.begin(g_config);
    web.begin(g_config, serial, net, proxy, /*captive=*/true);
    led.setMode(StatusLed::Mode::Portal);
    log_w("Offline — captive portal active");
    return;
  }

  log_i("Online via %s at %s", net.linkName(), net.localIP().toString().c_str());
  proxy.begin(g_config, serial);
  web.begin(g_config, serial, net, proxy, /*captive=*/false);
  ota.begin(g_config, web.raw());
  led.setMode(StatusLed::Mode::Online);
}

void loop() {
  captive.loop(); // no-op unless portal is active
  ota.loop();
  proxy.loop();

  // Flicker the status LED whenever a VISCA packet is proxied.
  static uint32_t seenIn = 0;
  uint32_t in = proxy.packetsIn();
  if (in != seenIn) {
    seenIn = in;
    led.blip();
  }
  led.loop();

  delay(1);
}
