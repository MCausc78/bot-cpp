#include "login_start.h"

LoginStart::LoginStart() = default;
LoginStart::LoginStart(int protocol, std::string name)
    : protocol(protocol), name(std::move(name)) {}
LoginStart::~LoginStart() = default;

void LoginStart::write(ByteBuffer& buf) const {
  buf.writeString(name);
  if (protocol >= 759) {
    buf.writeByte(0);
    if (protocol >= 760) {
      buf.writeByte(0);
    }
  }
}

int LoginStart::getID() const {
  return 0;
}
