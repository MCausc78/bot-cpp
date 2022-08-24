#pragma once

class Attack {
 public:
  Attack();
  virtual ~Attack();

 public:
  virtual void accept(int, int) = 0;
};