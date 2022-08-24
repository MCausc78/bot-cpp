#include "handshake.h"

#include <utility>

Handshake::Handshake() = default;
Handshake::~Handshake() = default;

Handshake::Handshake(int protocol, std::string host, int port, decltype(nextState) nextState)
    : protocol(protocol), host(std::move(host)), port(port), nextState(nextState) {}

void Handshake::write(ByteBuffer& buf) const {
  buf.writeVarInt(protocol);
  buf.writeString(host);
  buf.writeShort((short) port);
  buf.writeVarInt((int) nextState);
}

int Handshake::getID() const {
  return 0;
}
