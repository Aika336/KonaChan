#pragma once

#include "tcp.h"

#include <memory>
#include <openssl/ssl.h>

class TCPS : public TCP {
  using CtxPtr = std::unique_ptr<SSL_CTX, decltype(&SSL_CTX_free)>;
  using SslPtr = std::unique_ptr<SSL, decltype(&SSL_free)>;

  CtxPtr ctx{nullptr, SSL_CTX_free};
  SslPtr ssl{nullptr, SSL_free};

public:
  TCPS(const std::string &host, const std::string &port);
  ~TCPS() override = default;

  int sendA(const void *buffer, int len) override;
  int recvA(void *buffer, int len) override;
};
