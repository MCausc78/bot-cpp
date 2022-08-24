#pragma once

#include "../byte_buffer.h"
#include "attack.h"

class TcpHitAttack : public Attack {
 public:
  explicit TcpHitAttack(int protocol);
  ~TcpHitAttack() override;

 public:
  void accept(uv_stream_t *, const sockaddr_in *) override;

 private:
  ByteBuffer buf;
};