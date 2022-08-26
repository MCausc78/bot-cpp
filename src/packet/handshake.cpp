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

#include "handshake.h"

#include <utility>

Handshake::Handshake() = default;
Handshake::~Handshake() = default;

Handshake::Handshake(int protocol, std::string host, int port, decltype(nextState) nextState)
    : protocol(protocol), host(std::move(host)), port(port), nextState(nextState) {}

void Handshake::write(ByteBuffer& buf) const {
  buf.writeVarInt(protocol);
  buf.writeString(host);
  buf.writeShort((short) port);
  buf.writeVarInt((int) nextState);
}

int Handshake::getID() const {
  return 0;
}
