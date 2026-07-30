// SPDX-License-Identifier: AGPL-3.0-or-later
#include "ViscaSerial.h"
#include "config.h"

namespace ViscaCmd {

static inline uint8_t hdr(uint8_t addr) { return 0x80 | (addr & 0x0F); }

ViscaFrame panTilt(uint8_t addr, Pan pan, Tilt tilt, uint8_t panSpeed, uint8_t tiltSpeed) {
  // 8x 01 06 01 VV WW pp tt FF
  if (panSpeed < 0x01) panSpeed = 0x01;
  if (panSpeed > 0x18) panSpeed = 0x18; // max pan speed
  if (tiltSpeed < 0x01) tiltSpeed = 0x01;
  if (tiltSpeed > 0x14) tiltSpeed = 0x14; // max tilt speed
  return {hdr(addr), 0x01, 0x06, 0x01, panSpeed, tiltSpeed,
          (uint8_t)pan, (uint8_t)tilt, VISCA_TERMINATOR};
}

ViscaFrame panTiltStop(uint8_t addr) {
  return {hdr(addr), 0x01, 0x06, 0x01, 0x01, 0x01,
          (uint8_t)Pan::None, (uint8_t)Tilt::None, VISCA_TERMINATOR};
}

ViscaFrame home(uint8_t addr) {
  return {hdr(addr), 0x01, 0x06, 0x04, VISCA_TERMINATOR};
}

ViscaFrame zoom(uint8_t addr, int8_t dir, uint8_t speed) {
  // Stop: 8x 01 04 07 00 FF ; Tele: 2p ; Wide: 3p (p = speed 0..7)
  if (speed > 7) speed = 7;
  uint8_t b;
  if (dir > 0) b = 0x20 | speed;      // tele
  else if (dir < 0) b = 0x30 | speed; // wide
  else b = 0x00;                      // stop
  return {hdr(addr), 0x01, 0x04, 0x07, b, VISCA_TERMINATOR};
}

ViscaFrame focus(uint8_t addr, int8_t dir) {
  // Stop 00 ; Far 02 ; Near 03
  uint8_t b = dir > 0 ? 0x02 : (dir < 0 ? 0x03 : 0x00);
  return {hdr(addr), 0x01, 0x04, 0x08, b, VISCA_TERMINATOR};
}

ViscaFrame focusAuto(uint8_t addr, bool automatic) {
  return {hdr(addr), 0x01, 0x04, 0x38, (uint8_t)(automatic ? 0x02 : 0x03), VISCA_TERMINATOR};
}

ViscaFrame power(uint8_t addr, bool on) {
  return {hdr(addr), 0x01, 0x04, 0x00, (uint8_t)(on ? 0x02 : 0x03), VISCA_TERMINATOR};
}

ViscaFrame presetSet(uint8_t addr, uint8_t id) {
  return {hdr(addr), 0x01, 0x04, 0x3F, 0x01, (uint8_t)(id & 0x7F), VISCA_TERMINATOR};
}

ViscaFrame presetRecall(uint8_t addr, uint8_t id) {
  return {hdr(addr), 0x01, 0x04, 0x3F, 0x02, (uint8_t)(id & 0x7F), VISCA_TERMINATOR};
}

} // namespace ViscaCmd

void ViscaSerial::begin(const AppConfig &cfg) {
  if (!_mutex) _mutex = xSemaphoreCreateMutex();
  _serial.begin(cfg.serialBaud, SERIAL_8N1, cfg.uartRx, cfg.uartTx);
}

bool ViscaSerial::lock(uint32_t timeoutMs) {
  if (!_mutex) return true;
  return xSemaphoreTake(_mutex, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
}

void ViscaSerial::unlock() {
  if (_mutex) xSemaphoreGive(_mutex);
}

void ViscaSerial::write(const uint8_t *data, size_t len) {
  _serial.write(data, len);
  _serial.flush();
}

ViscaFrame ViscaSerial::readReply(uint32_t timeoutMs) {
  ViscaFrame frame;
  uint32_t start = millis();
  while (millis() - start < timeoutMs) {
    while (_serial.available()) {
      uint8_t b = _serial.read();
      frame.push_back(b);
      if (b == VISCA_TERMINATOR) return frame;
      if (frame.size() >= VISCA_MAX_PACKET) return frame; // safety cap
    }
    delay(1);
  }
  return frame; // may be empty (timeout) or partial
}

ViscaFrame hexToBytes(const String &hex) {
  ViscaFrame out;
  int hi = -1;
  for (size_t i = 0; i < hex.length(); i++) {
    char c = hex[i];
    int v;
    if (c >= '0' && c <= '9') v = c - '0';
    else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
    else continue; // skip spaces, commas, 0x, etc.
    if (hi < 0) hi = v;
    else {
      out.push_back((uint8_t)((hi << 4) | v));
      hi = -1;
    }
  }
  return out;
}

String bytesToHex(const uint8_t *data, size_t len) {
  String s;
  s.reserve(len * 3);
  const char *digits = "0123456789ABCDEF";
  for (size_t i = 0; i < len; i++) {
    if (i) s += ' ';
    s += digits[data[i] >> 4];
    s += digits[data[i] & 0x0F];
  }
  return s;
}