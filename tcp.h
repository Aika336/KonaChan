#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

class TCP {
protected:
  addrinfo hint{};
  int fd{-1};

  void Connection(const std::string &host, const std::string &port);

public:
  TCP(const std::string &host, const std::string &port);
  virtual ~TCP();

  int getFd();
  virtual int sendA(const void *buffer, int len);
  virtual int recvA(void *buffer, int len);
};
