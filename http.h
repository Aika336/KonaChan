#pragma once
#include "tcp.h"
#include "url.h"
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

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
  if (u.scheme != "http") {
    printf("Only http\n");
    exit(1);
  }
  TCP tcp(u.host, u.port);
  int fd = tcp.getFd();
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
  size_t sent = 0;
  while (sent < request.size()) {
    ssize_t n =
        send(fd, request.data() + sent, request.size() - sent, MSG_NOSIGNAL);
    if (n <= 0) {
      close(fd);
      printf("send() failed\n");
      exit(1);
    }
    sent += static_cast<size_t>(n);
  }

  // Receive until server closes
  std::string response;
  char buf[8192];
  while (true) {
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n == 0)
      break;
    if (n < 0) {
      close(fd);
      printf("recv() failed\n");
      exit(1);
    }
    response.append(buf, static_cast<size_t>(n));
  }
  close(fd);
  return response;
}
