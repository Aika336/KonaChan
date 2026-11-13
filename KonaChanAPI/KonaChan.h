#pragma once

#include "http.h"

#include <string>
#include <vector>

class KonaChan {
  static const std::string base_url_;

public:
  KonaChan() = default;
  void downloadByTagsLimit(const std::vector<std::string> &tags, int limit);
};
