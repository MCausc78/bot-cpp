#include "packet.h"

Packet::Packet() = default;
Packet::~Packet() = default;

void Packet::writeWrapped(ByteBuffer& buf) {
  ByteBuffer unwrapped;
  unwrapped.writeVarInt(getID());
  write(unwrapped);

  buf.writeVarInt((int) unwrapped.readableBytes());
  buf.writeBytes(unwrapped);
}
