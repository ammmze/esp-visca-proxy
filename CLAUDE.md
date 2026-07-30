# CLAUDE.md

Guidance for Claude Code working in this repo.

## What this is

ESP32 firmware (PlatformIO/Arduino) that bridges **Sony VISCA-over-IP** (UDP
port 52381) to an **RS232** PTZ camera through a MAX3232 TTL↔RS232 board.
Optional W5500 Ethernet, WiFi with captive-portal setup fallback, OTA (web +
espota), and a web UI for PTZ control.

## Build / flash

```
pio run                      # build default env (esp32-s3)
pio run -e esp32dev          # build a specific variant
pio run -e esp32-s3 -t upload
pio device monitor
```

Envs: `esp32-s3` (default), `esp32dev`, `esp32-c3`, `esp32-s2`.

## Layout

- `src/` — firmware modules (each feature = a `.h`/`.cpp` pair):
  `AppConfig`, `NetLink`, `CaptivePortal`, `ViscaSerial`, `ViscaProxy`,
  `WebServer`, `OtaManager`, `StatusLed`, `main.cpp`.
- `include/config.h` — per-board default pins (W5500 SPI, UART) + constants.
- `data/` — web UI source (html/js/css).
- `scripts/embed_assets.py` — pre-build: gzips `data/*` into `src/web_assets.h`.
- Config persists in NVS (`Preferences`); web assets are embedded in firmware.

## Conventions

- Runtime-configurable pins/baud/network live in `AppConfig`; compile-time
  defaults live in `include/config.h`.
- `src/web_assets.h` is generated — never edit by hand; edit `data/` instead.
