#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

class TCP {
  addrinfo hint{};
  int fd;

public:
  TCP(const std::string &host, const std::string &port);
  ~TCP();
  int getFd();
};
