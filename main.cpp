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

#include "http.h"
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <url> [output_file]\n";
    return 1;
  }
  std::string url = argv[1];
  std::string out = (argc > 2) ? argv[2] : "download.bin";

  try {
    downloadFile(url, out);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
