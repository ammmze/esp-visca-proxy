# esp-visca-proxy

ESP32 firmware that bridges **Sony VISCA-over-IP** (UDP port `52381`) to an
**RS232** PTZ camera through a MAX3232 TTL↔RS232 board — plus a web UI to drive
the camera directly.

## Features

- Runs on multiple ESP32 variants (ESP32-S3 default; classic ESP32, C3, S2).
- WiFi **or** optional **W5500 Ethernet** (SPI). The web server and UDP proxy
  work over whichever link comes up.
- **Captive portal** for first-time setup when no network is configured.
- **VISCA-over-IP → RS232** proxy (8-byte Sony header, sequence tracking, reply
  forwarding).
- **Web UI**: PTZ joystick pad, pan/tilt/zoom/focus, presets (store/recall),
  raw VISCA hex console, and settings.
- **OTA** updates: browser upload (`/update`) and `espota`/IDE push (ArduinoOTA).
- Config stored in NVS; web UI embedded in firmware (single-binary OTA).
- **CI** builds all variants and attaches firmware to tagged releases.

## Build

```
pio run                       # default env (esp32-s3)
pio run -e esp32dev           # a specific variant
pio run -e esp32-s3 -t upload # flash over USB
pio device monitor
```

Environments: `esp32-s3`, `esp32dev`, `esp32-c3`, `esp32-s2`.

## First-time setup

1. Flash the firmware. With no WiFi/Ethernet configured, the device starts an
   AP named **`ESP-VISCA-Setup`**.
2. Join it, open any page (captive portal redirects to the UI), go to
   **Settings**, enter WiFi (and/or enable Ethernet), pin mapping, VISCA port,
   camera address, then **Save & Reboot**.
3. After reboot, browse to the device IP (shown in the status badge / your
   router). VISCA-over-IP controllers can now target `<device-ip>:52381`.

## Wiring

- **MAX3232**: `UART TX/RX` (see Settings for per-board defaults) → MAX3232 TTL
  side; RS232 side → camera VISCA port.
- **W5500** (optional): SPI `SCK/MISO/MOSI/CS` + `INT`/`RST`. Defaults are
  per-board and overridable in Settings.

## Wiki / internals

See [CLAUDE.md](CLAUDE.md) for layout and conventions. Web assets live in
`data/` and are gzipped into `src/web_assets.h` at build time by
`scripts/embed_assets.py` — never edit the generated header.

## License

Licensed under the **GNU Affero General Public License v3.0 or later**
(AGPL-3.0-or-later) — see [LICENSE](LICENSE). This is required because the
firmware links the AGPL-licensed [ElegantOTA](https://github.com/ayushsharma82/ElegantOTA)
library; the whole distributed image is therefore AGPL.

Because the device serves a web UI, AGPL §13 applies: if you modify and deploy
it, you must offer users the corresponding source. Update `SOURCE_URL` in
[include/config.h](include/config.h) to point at your published source — it is
surfaced as the "Source" link in the web UI.

> Note: AGPL-3.0 permits commercial use. If you need to prohibit commercial use,
> that requires replacing the AGPL dependency (ElegantOTA) and applying a
> source-available license such as PolyForm Noncommercial.

Bundled third-party libraries retain their own licenses: ArduinoJson (MIT),
AsyncTCP & ESPAsyncWebServer (LGPL-3.0), ElegantOTA (AGPL-3.0), and the
Arduino-ESP32 core (LGPL-2.1 / Apache-2.0).
