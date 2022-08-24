#pragma once

#include "../byte_buffer.h"
#include "attack.h"

class JoinAttack : public Attack {
 public:
  JoinAttack(int, const std::string&, int);
  ~JoinAttack() override;

 public:
  void accept(uv_stream_t *, const sockaddr_in *) override;

 private:
  ByteBuffer handshake;
  int protocol;
};