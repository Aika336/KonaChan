#pragma once
#include "tcp.h"
#include "tcps.h"
#include "url.h"

#include "algorithm"
#include <fstream>
#include <iostream>
#include <memory>
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

  std::unique_ptr<TCP> tcp;
  if (u.scheme == "http") {
    tcp = std::make_unique<TCP>(u.host, u.port);
  } else if (u.scheme == "https") {
    tcp = std::make_unique<TCPS>(u.host, u.port);
  } else {
    printf("Unsupport scheme: %s\n", u.scheme);
    exit(1);
  }

  int fd = tcp->getFd();
  if (fd < 0) {
    printf("connection failed\n");
    exit(1);
  }

  std::ostringstream req;
  req << "GET " << u.path << " HTTP/1.1\r\n"
      << "Host: " << u.host << "\r\n"
      << "User-Agent: SimpleHttpClient/1.0\r\n"
      << "Accept: */*\r\n"
      << "Connection: close\r\n" // server will close when done
      << "\r\n";
  std::string request = req.str();

  int sent = 0;
  while (sent < request.size()) {
    int n = tcp->sendA(request.data() + sent, request.size() - sent);
    if (n <= 0) {
      printf("send() failed\n");
      exit(1);
    }
    sent += (n);
  }

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

int getStatusCode(const std::string &status_line) {
  auto sp = status_line.find(' ');
  if (sp == std::string::npos)
    return 0;
  try {
    return std::stoi(status_line.substr(sp + 1));
  } catch (...) {
    return 0;
  }
}

std::string getHeader(const std::string &head, const std::string &name) {
  std::string lower_name = name;
  std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
                 ::tolower);

  size_t pos = 0;
  while (pos < head.size()) {
    size_t nl = head.find("\r\n", pos);
    std::string line = (nl == std::string::npos) ? head.substr(pos)
                                                 : head.substr(pos, nl - pos);

    // Сравниваем префикс "name:" без учёта регистра
    if (line.size() > lower_name.size() + 1) {
      std::string prefix = line.substr(0, lower_name.size() + 1);
      std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);
      if (prefix == lower_name + ":") {
        std::string val = line.substr(lower_name.size() + 1);
        size_t s = val.find_first_not_of(" \t");
        if (s == std::string::npos)
          return "";
        size_t e = val.find_last_not_of(" \t\r");
        return val.substr(s, e - s + 1);
      }
    }
    if (nl == std::string::npos)
      break;
    pos = nl + 2;
  }
  return "";
}

inline std::string decodeChunked(const std::string &in) {
  std::string out;
  size_t pos = 0;
  while (pos < in.size()) {
    auto line_end = in.find("\r\n", pos);
    if (line_end == std::string::npos)
      break;

    std::string size_line = in.substr(pos, line_end - pos);

    auto semi = size_line.find(';');
    if (semi != std::string::npos)
      size_line.resize(semi);

    size_t chunk_size = 0;
    try {
      chunk_size = std::stoul(size_line, nullptr, 16);
    } catch (...) {
      break;
    }

    pos = line_end + 2;
    if (chunk_size == 0)
      break;
    if (pos + chunk_size + 2 > in.size())
      break;
    out.append(in, pos, chunk_size);
    pos += chunk_size + 2;
  }
  return out;
}

inline std::string resolveUrl(const std::string &base, const std::string &loc) {
  if (loc.find("://") != std::string::npos)
    return loc; // абсолютный

  Url u = parseUrl(base);
  std::string hostPort = u.host;
  if (!((u.scheme == "http" && u.port == "80") ||
        (u.scheme == "https" && u.port == "443")))
    hostPort += ":" + u.port;

  if (!loc.empty() && loc[0] == '/')
    return u.scheme + "://" + hostPort + loc;

  auto last = u.path.rfind('/');
  std::string dir =
      (last == std::string::npos) ? "/" : u.path.substr(0, last + 1);
  return u.scheme + "://" + hostPort + dir + loc;
}

void downloadFile(const std::string &url, const std::string &name) {
  std::string current_url = url;

  for (;;) {
    std::string raw = httpGet(current_url);
    HttpResponse res = parseResponse(raw);
    int code = getStatusCode(res.status);

    if (code >= 300 && code < 400) {
      std::string loc = getHeader(res.head, "Location");
      if (loc.empty())
        throw std::runtime_error("Redirect " + std::to_string(code) +
                                 " without Location header");
      current_url = resolveUrl(current_url, loc);
      continue;
    }

    std::string body = res.body;
    std::string te = getHeader(res.body, "Transfer-Encoding");
    if (te.find("chunked") != std::string::npos) {
      body = decodeChunked(body);
    }

    std::ofstream out(name, std::ios::binary | std::ios::trunc);
    if (!out)
      throw std::runtime_error("Cannot open " + name);

    out.write(body.data(), static_cast<std::streamsize>(body.size()));
    out.close();

    std::cout << "Saved " << body.size() << " bytes to " << name << " (from "
              << current_url << ")\n";
    return;
  }
  return;
}
