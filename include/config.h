// SPDX-License-Identifier: AGPL-3.0-or-later
// config.h — compile-time constants and per-board default pin maps.
// All pins are runtime-overridable via AppConfig / the web UI; these are just
// the factory defaults offered on first boot.
#pragma once

// ---- Firmware identity ----------------------------------------------------
#define FW_NAME "esp-visca-proxy"
#define FW_VERSION "0.3.0" // x-release-please-version
// AGPL-3.0 §13: this network-facing app must offer its source to users. Shown
// as a "Source" link in the web UI — update if you fork/relocate the repo.
#define SOURCE_URL "https://github.com/ammmze/esp-visca-proxy"

// ---- Network / protocol defaults ------------------------------------------
#define DEFAULT_VISCA_UDP_PORT 52381 // Sony VISCA-over-IP standard port
#define DEFAULT_VISCA_ADDRESS 1      // camera address on the VISCA bus
#define DEFAULT_PRESET_COUNT 6       // preset buttons shown in the web UI
#define DEFAULT_SERIAL_BAUD 9600     // VISCA default; 38400 also common
// Base names; a "-<deviceId>" suffix (from the chip MAC) is appended at runtime
// so multiple devices get unique defaults. See AppConfig::load().
#define DEFAULT_HOSTNAME "visca-proxy"
#define DEFAULT_AP_SSID "ESP-VISCA-Setup"
#define DEFAULT_AP_PASSWORD "" // empty = open AP for first-time setup

#define NET_CONNECT_TIMEOUT_MS 15000 // per-link connect wait before fallback
#define VISCA_SERIAL_TIMEOUT_MS 1000 // wait for a camera reply frame
#define VISCA_MAX_PACKET 32          // max VISCA payload bytes
#define VISCA_TERMINATOR 0xFF        // VISCA packet terminator byte

// ---- Per-board default pins -----------------------------------------------
// W5500 uses SPI: SCK/MISO/MOSI/CS plus INT and RST control lines.
// UART_TX/UART_RX drive the MAX3232 TTL<->RS232 board.
#if defined(BOARD_ESP32_S3)
#define DEF_ETH_SCK 12
#define DEF_ETH_MISO 13
#define DEF_ETH_MOSI 11
#define DEF_ETH_CS 10
#define DEF_ETH_INT 9
#define DEF_ETH_RST 14
#define DEF_UART_TX 17
#define DEF_UART_RX 18

#elif defined(BOARD_ESP32_C3)
#define DEF_ETH_SCK 4
#define DEF_ETH_MISO 5
#define DEF_ETH_MOSI 6
#define DEF_ETH_CS 7
#define DEF_ETH_INT 3
#define DEF_ETH_RST 2
#define DEF_UART_TX 21
#define DEF_UART_RX 20

#elif defined(BOARD_ESP32_S2)
#define DEF_ETH_SCK 36
#define DEF_ETH_MISO 37
#define DEF_ETH_MOSI 35
#define DEF_ETH_CS 34
#define DEF_ETH_INT 33
#define DEF_ETH_RST 38
#define DEF_UART_TX 17
#define DEF_UART_RX 18

#else // BOARD_ESP32_CLASSIC / default
#define DEF_ETH_SCK 18
#define DEF_ETH_MISO 19
#define DEF_ETH_MOSI 23
#define DEF_ETH_CS 5
#define DEF_ETH_INT 4
#define DEF_ETH_RST 15
#define DEF_UART_TX 17
#define DEF_UART_RX 16
#endif

// ---- Status LED default ---------------------------------------------------
// Prefer the board's LED_BUILTIN (the core routes digitalWrite to the RGB LED
// on devkits that only have one). Fall back to GPIO2 on classic boards; -1
// disables. Runtime-overridable via AppConfig / the web UI.
// (Requires <Arduino.h> to be included before this header.)
#if defined(LED_BUILTIN)
#define DEF_STATUS_LED LED_BUILTIN
#elif defined(BOARD_ESP32_CLASSIC)
#define DEF_STATUS_LED 2
#else
#define DEF_STATUS_LED -1
#endif
#define DEF_STATUS_LED_ACTIVE_LOW 0