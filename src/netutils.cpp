#include "netutils.h"

#include <sstream>

void write(uv_stream_t* stream, const ByteBuffer& buf) {
  uv_buf_t uvBuf[] = {{.base = (char*) buf.data.data(), .len = buf.length()}};
  auto* req = (uv_write_t*) malloc(sizeof(uv_write_t));
  uv_write(req, stream, uvBuf, 1, [](uv_write_t* req, int s) {
    if (s < 0) {
      std::cerr << "Каннот сенд дата ту зе сервер" << std::endl;
    }
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