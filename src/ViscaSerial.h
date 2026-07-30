// SPDX-License-Identifier: AGPL-3.0-or-later
// ViscaSerial — UART link to the MAX3232/RS232 camera plus VISCA frame I/O and
// command builders used by the web UI.
#pragma once

#include <Arduino.h>
#include <vector>
#include "AppConfig.h"

using ViscaFrame = std::vector<uint8_t>;

// Pure command builders. `addr` is the VISCA bus address (1..7); the header
// byte becomes 0x80 | addr. Each returns a complete frame terminated by 0xFF.
namespace ViscaCmd {
enum class Pan : uint8_t { Left = 0x01, Right = 0x02, None = 0x03 };
enum class Tilt : uint8_t { Up = 0x01, Down = 0x02, None = 0x03 };

ViscaFrame panTilt(uint8_t addr, Pan pan, Tilt tilt, uint8_t panSpeed, uint8_t tiltSpeed);
ViscaFrame panTiltStop(uint8_t addr);
ViscaFrame home(uint8_t addr);
ViscaFrame zoom(uint8_t addr, int8_t dir, uint8_t speed); // dir: +tele, -wide, 0 stop
ViscaFrame focus(uint8_t addr, int8_t dir);               // dir: +far, -near, 0 stop
ViscaFrame focusAuto(uint8_t addr, bool automatic);
ViscaFrame power(uint8_t addr, bool on);
ViscaFrame presetSet(uint8_t addr, uint8_t id);
ViscaFrame presetRecall(uint8_t addr, uint8_t id);
} // namespace ViscaCmd

class ViscaSerial {
public:
  void begin(const AppConfig &cfg);

  // Write raw VISCA bytes to the camera.
  void write(const uint8_t *data, size_t len);
  void write(const ViscaFrame &frame) { write(frame.data(), frame.size()); }

  // Read one reply frame (bytes up to and including the 0xFF terminator).
  // Returns the frame, or empty on timeout.
  ViscaFrame readReply(uint32_t timeoutMs);

  // Serialize a whole write+read transaction — the proxy loop and the web
  // handlers run in different tasks and share this UART. Wrap each exchange in
  // lock()/unlock().
  bool lock(uint32_t timeoutMs = 2000);
  void unlock();

  HardwareSerial &raw() { return _serial; }

private:
  HardwareSerial _serial{1}; // UART1
  SemaphoreHandle_t _mutex = nullptr;
};

// Hex helpers for the raw-command console.
ViscaFrame hexToBytes(const String &hex);
String bytesToHex(const uint8_t *data, size_t len);