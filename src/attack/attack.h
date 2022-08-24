#pragma once

#include <uv.h>
class Attack {
 public:
  Attack();
  virtual ~Attack();

 public:
  virtual void accept(uv_stream_t*, const sockaddr_in*) = 0;
};