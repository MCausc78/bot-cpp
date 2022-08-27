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

#include "netutils.h"

#include <sstream>

void write(uv_stream_t* stream, const ByteBuffer& buf) {
  uv_buf_t uvBuf[] = {{.base = (char*) buf.data.data(), .len = buf.length()}};
  auto* req = (uv_write_t*) malloc(sizeof(uv_write_t));
  uv_write(req, stream, uvBuf, 1, [](uv_write_t* req, int s) {
//    if (s < 0) {
//      std::cerr << "Каннот сенд дата ту зе сервер" << std::endl;
//    }
    free(req);
  });
}

void splitIP(const std::string& source, std::string& ip, int& port) {
  std::stringstream ss;
  ss << source;
  getline(ss, ip, ':');
  ss >> port;
  if (port == 0) {
    port = 25565;
  }
}