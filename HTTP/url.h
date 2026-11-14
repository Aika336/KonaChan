#pragma once
#include <string>

struct Url {
  std::string scheme;
  std::string host;
  std::string port;
  std::string path;
};

inline Url parseUrl(const std::string &url) {
  Url u;
  auto scheme_end = url.find("://");
  if (scheme_end == std::string::npos) {
    printf("Invalid URL: missing scheme\n");
    exit(1);
  }

  u.scheme = url.substr(0, scheme_end);

  std::string rest = url.substr(scheme_end + 3);
  auto path_pos = rest.find('/');
  std::string host_port;
  if (path_pos == std::string::npos) {
    host_port = rest;
    u.path = "/";
  } else {
    host_port = rest.substr(0, path_pos);
    u.path = rest.substr(path_pos);
  }

  auto colon = host_port.find(':');
  if (colon == std::string::npos) {
    u.host = host_port;
    u.port = (u.scheme == "https") ? "443" : "80";
  } else {
    u.host = host_port.substr(0, colon);
    u.port = host_port.substr(colon + 1);
  }
  return u;
}
