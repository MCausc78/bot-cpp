#pragma once

#include "packet.h"

class Handshake : public Packet {
 public:
  Handshake();
  ~Handshake() override;

 public:
  void write(ByteBuffer&) const override;
  [[nodiscard]] int getID() const override;

 public:
  int protocol = 0;
  std::string host;
  int port = 0;
  enum {
    STATUS = 1,
    LOGIN = 2,
  } nextState = STATUS;

 public:
  Handshake(int, std::string , int, decltype(nextState));
};