#include "tcp.h"

TCP::TCP(const std::string &host, const std::string &port) {
  Connection(host, port);
}

void TCP::Connection(const std::string &host, const std::string &port) {
  hint.ai_family = AF_UNSPEC;
  hint.ai_socktype = SOCK_STREAM;
  fd = -1;

  addrinfo *result = nullptr;
  int rc = getaddrinfo(host.c_str(), port.c_str(), &hint, &result);
  if (rc != 0) {
    printf("Get address info ERROR: %d", rc);
    fd = -1;
    return;
  }
  for (addrinfo *p = result; p != nullptr; p = p->ai_next) {
    int fc = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (fc == -1)
      continue;

    if (connect(fc, p->ai_addr, p->ai_addrlen) == 0) {
      fd = fc;
      break;
    }
    close(fc);
  }

  freeaddrinfo(result);
}

TCP::~TCP() {
  if (fd >= 0) {
    close(fd);
  }
}

int TCP::sendA(const void *buffer, int len) {
  return static_cast<int>(send(fd, buffer, len, MSG_NOSIGNAL));
}

int TCP::recvA(void *buffer, int len) {
  return static_cast<int>(recv(fd, buffer, len, 0));
}

int TCP::getFd() { return fd; }
