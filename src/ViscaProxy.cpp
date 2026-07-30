#include "ViscaProxy.h"
#include "config.h"

static constexpr uint16_t TYPE_COMMAND = 0x0100;
static constexpr uint16_t TYPE_INQUIRY = 0x0110;
static constexpr uint16_t TYPE_REPLY = 0x0111;
static constexpr uint16_t TYPE_CONTROL = 0x0200;
static constexpr uint16_t TYPE_CONTROL_REPLY = 0x0201;

static constexpr size_t MAX_QUEUE = 16;

void ViscaProxy::begin(const AppConfig &cfg, ViscaSerial &serial) {
  _serial = &serial;
  if (_udp.listen(cfg.viscaPort)) {
    _udp.onPacket([this](AsyncUDPPacket packet) {
      const uint8_t *d = packet.data();
      size_t len = packet.length();
      if (len < 8) return; // malformed: need at least the header

      Pending p;
      p.ip = packet.remoteIP();
      p.port = packet.remotePort();
      p.type = (uint16_t)((d[0] << 8) | d[1]);
      uint16_t payloadLen = (uint16_t)((d[2] << 8) | d[3]);
      p.seq = ((uint32_t)d[4] << 24) | ((uint32_t)d[5] << 16) |
              ((uint32_t)d[6] << 8) | (uint32_t)d[7];
      // Clamp payload length to what actually arrived.
      if (payloadLen > len - 8) payloadLen = len - 8;
      p.payload.assign(d + 8, d + 8 + payloadLen);

      if (_queue.size() < MAX_QUEUE) _queue.push_back(std::move(p));
      _packetsIn++;
    });
    log_i("VISCA-over-IP listening on UDP %u", cfg.viscaPort);
  } else {
    log_e("Failed to bind VISCA UDP port %u", cfg.viscaPort);
  }
}

void ViscaProxy::loop() {
  if (_queue.empty() || !_serial) return;
  Pending p = std::move(_queue.front());
  _queue.pop_front();
  handle(p);
}

void ViscaProxy::handle(const Pending &p) {
  // Control messages (e.g. reset sequence number) are answered locally; they
  // are not VISCA payloads for the camera.
  if (p.type == TYPE_CONTROL) {
    // 0x01 = RESET sequence number, 0x0F 0x01 = error, etc. Ack with reply.
    uint8_t ack[1] = {0x01};
    sendPacket(p.ip, p.port, TYPE_CONTROL_REPLY, p.seq, ack, 1);
    return;
  }

  if (p.type != TYPE_COMMAND && p.type != TYPE_INQUIRY) return;
  if (p.payload.empty()) return;

  if (!_serial->lock()) return;

  // Forward VISCA payload to the camera over RS232.
  _serial->write(p.payload.data(), p.payload.size());

  // A camera answers with one or more frames (ACK, then completion, or an
  // inquiry response). Forward each back with the request's sequence number.
  bool first = true;
  while (true) {
    uint32_t timeout = first ? VISCA_SERIAL_TIMEOUT_MS : 200;
    ViscaFrame reply = _serial->readReply(timeout);
    if (reply.empty()) break;
    sendPacket(p.ip, p.port, TYPE_REPLY, p.seq, reply.data(), reply.size());
    first = false;
    // Stop after a completion (0x90 0x5y) or error terminator with no more data.
    if (reply.size() >= 2 && (reply[1] & 0xF0) == 0x50) break;
  }

  _serial->unlock();
}

void ViscaProxy::sendPacket(const IPAddress &ip, uint16_t port, uint16_t type,
                            uint32_t seq, const uint8_t *payload, size_t len) {
  uint8_t buf[8 + VISCA_MAX_PACKET];
  if (len > VISCA_MAX_PACKET) len = VISCA_MAX_PACKET;
  buf[0] = type >> 8;
  buf[1] = type & 0xFF;
  buf[2] = len >> 8;
  buf[3] = len & 0xFF;
  buf[4] = seq >> 24;
  buf[5] = seq >> 16;
  buf[6] = seq >> 8;
  buf[7] = seq & 0xFF;
  memcpy(buf + 8, payload, len);
  _udp.writeTo(buf, 8 + len, ip, port);
  _packetsOut++;
}
