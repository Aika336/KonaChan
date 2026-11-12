#pragma once
#include "tcp.h"
#include "tcps.h"
#include "url.h"

#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <memory>

struct HttpResponse {
  std::string status;
  std::string head;
  std::string body;
};

HttpResponse parseResponse(const std::string &raw) {
  HttpResponse r;
  auto header_end = raw.find("\r\n\r\n");

  if (header_end == std::string::npos) {
    printf("Malformed HTTPS response\n");
    exit(1);
  }

  std::string head = raw.substr(0, header_end);
  r.body = raw.substr(header_end + 4);

  auto first_nl = head.find("\r\n");
  r.status = head.substr(0, first_nl);

  if (first_nl != std::string::npos) {
    r.head = head.substr(first_nl + 2);
  }

  return r;
}

std::string httpGet(const std::string &url) {
  Url u = parseUrl(url);

    std::unique_ptr<TCP> tcp;
  if (u.scheme == "http") {
      tcp = std::make_unique<TCP>(u.host, u.port);
  }else if(u.scheme == "https") {
        tcp = std::make_unique<TCPS>(u.host, u.port);
  }else {
    printf("Unsupport scheme: %s\n", u.scheme);
    exit(1);
  }
  
  int fd = tcp->getFd();
  if (fd < 0) {
    printf("connection failed\n");
    exit(1);
  }

  // Build request
  std::ostringstream req;
  req << "GET " << u.path << " HTTP/1.1\r\n"
      << "Host: " << u.host << "\r\n"
      << "User-Agent: SimpleHttpClient/1.0\r\n"
      << "Accept: */*\r\n"
      << "Connection: close\r\n" // server will close when done
      << "\r\n";
  std::string request = req.str();

  // Send (loop until all bytes sent, MSG_NOSIGNAL avoids SIGPIPE)
  int sent = 0;
  while (sent < request.size()) {
    int n = tcp->sendA(request.data() + sent, request.size() - sent);
    if (n <= 0) {
      printf("send() failed\n");
      exit(1);
    }
    sent += (n);
  }

  // Receive until server closes
  std::string response;
  char buf[8192];
  while (true) {
    int n = tcp->recvA(buf, sizeof(buf));
    if (n == 0)
      break;
    if (n < 0) {
      close(fd);
      printf("recv() failed\n");
      exit(1);
    }
    response.append(buf, static_cast<size_t>(n));
  }

  return response;
}
