#pragma once

#include "http.h"
#include "json.hpp"

#include <string>
#include <vector>

struct PostData {
  int id;
  std::string file_url;
  std::string tags;
  int height;
  int width;
  std::string ext;
};

class KonaChan {
public:
  static const std::string base_url_;
  std::string makeUrl(const std::vector<std::string> &tags, int limit);
  std::string getJsonFromUrl(const std::string &url);
  std::vector<PostData> parseJsonByString(const std::string &json_str);

public:
  KonaChan() = default;
};
