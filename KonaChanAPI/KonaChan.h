#pragma once

#include "http.h"
#include "json.hpp"

#include <string>
#include <vector>

class KonaChan {
  static const std::string base_url_;
  std::string makeUrl(const std::vector<std::string> &tags, int limit);
  std::string getJsonFromUrl(const std::string &url);
  json parseJsonByString(const std::string &str);

public:
  KonaChan() = default;
};
