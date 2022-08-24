#pragma once

#include "../byte_buffer.h"
class Packet {
 public:
  Packet();
  virtual ~Packet();

 public:
  virtual void write(ByteBuffer&) const = 0;
  [[nodiscard]] virtual int getID() const = 0;
  void writeWrapped(ByteBuffer&);
};