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

#include "ping.h"

#include "../netutils.h"
#include "../packet/handshake.h"
#include "../packet/status_request.h"

PingAttack::PingAttack(int protocol, const std::string& ip, int port) {
  Handshake(protocol, ip, port, Handshake::STATUS).writeWrapped(buf);
  StatusRequest().writeWrapped(buf);
}

PingAttack::~PingAttack() = default;

void PingAttack::accept(uv_stream_t* stream, const sockaddr_in*) {
  write(stream, buf);
}