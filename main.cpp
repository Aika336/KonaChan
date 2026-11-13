/* #include "http.h"

#include <iostream>

int main(int argc, char **argv) {
  std::string url = (argc > 1) ? argv[1] : "http://example.com/";

  try {
    std::string raw = httpGet(url);
    HttpResponse res = parseResponse(raw);

    std::cout << "=== Status ===\n"
              << res.status << "\n\n"
              << "=== Headers ===\n"
              << res.head << "\n\n"
              << "=== Body ===\n"
              << res.body << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}*/

#include "KonaChanAPI/KonaChan.h"
#include <iostream>

int main(int argc, char **argv) {
  KonaChan k;
  std::vector<std::string> tags = {"white", "nun"};
  int limit = 5;
  k.downloadByTagsLimit(tags, limit);
  return 0;
}
