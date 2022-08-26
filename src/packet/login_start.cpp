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
