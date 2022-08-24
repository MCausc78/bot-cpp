#pragma once

#include "packet.h"
class LoginStart : public Packet {
 public:
  LoginStart();
  LoginStart(int, std::string);
  ~LoginStart() override;

 public:
  void write(ByteBuffer &) const override;
  [[nodiscard]] int getID() const override;

 public:
  std::string name;
  int protocol = 0;
};