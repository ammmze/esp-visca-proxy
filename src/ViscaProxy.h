// SPDX-License-Identifier: AGPL-3.0-or-later
// ViscaProxy — Sony VISCA-over-IP (UDP) <-> RS232 bridge.
//
// VISCA-over-IP framing (8-byte header, big-endian):
//   [payloadType:2][payloadLength:2][sequence:4][ payload... ]
// Payload types: 0x0100 command, 0x0110 inquiry, 0x0111 reply,
//                0x0200 control, 0x0201 control reply.
#pragma once

#include <Arduino.h>
#include <AsyncUDP.h>
#include <deque>
#include "AppConfig.h"
#include "ViscaSerial.h"

class ViscaProxy {
public:
  void begin(const AppConfig &cfg, ViscaSerial &serial);
  void loop(); // drain queued packets, do serial I/O (called from main loop)

  uint32_t packetsIn() const { return _packetsIn; }
  uint32_t packetsOut() const { return _packetsOut; }

private:
  struct Pending {
    IPAddress ip;
    uint16_t port;
    uint16_t type;
    uint32_t seq;
    ViscaFrame payload;
  };

  void handle(const Pending &p);
  void sendPacket(const IPAddress &ip, uint16_t port, uint16_t type,
                  uint32_t seq, const uint8_t *payload, size_t len);

  AsyncUDP _udp;
  ViscaSerial *_serial = nullptr;
  std::deque<Pending> _queue;
  uint32_t _packetsIn = 0;
  uint32_t _packetsOut = 0;
};