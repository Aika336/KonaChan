#include "tcps.h"

#include <openssl/err.h>
#include <stdexcept>

TCPS::TCPS(const std::string &host, const std::string &port) : TCP(host, port) {
  if (fd < 0)
    return;

  ctx.reset(SSL_CTX_new(TLS_client_method()));
  if (!ctx)
    throw std::runtime_error("SSL_CTX_new failed");

  SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_PEER, nullptr);

  ssl.reset(SSL_new(ctx.get()));
  if (!ssl)
    throw std::runtime_error("SSL_new failed");

  SSL_set_fd(ssl.get(), fd);
  SSL_set_tlsext_host_name(ssl.get(), host.c_str()); // SNI
  SSL_set1_host(ssl.get(), host.c_str());            // verify hostname

  if (SSL_connect(ssl.get()) <= 0) {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("SSL_connect failed");
  }
}

int TCPS::sendA(const void *buffer, int len) {
  return SSL_write(ssl.get(), buffer, len);
}

int TCPS::recvA(void *buffer, int len) {
  return SSL_read(ssl.get(), buffer, len);
}
