#include "KonaChan.h"

#include <iostream>
#include <stdexcept>

const std::string KonaChan::base_url_ = "https://konachan.net/post.json?";

std::string KonaChan::makeUrl(const std::vector<std::string> &tags, int limit) {
  std::string url{base_url_};
  url.append("tags=");

  bool first = true;
  for (const auto &tag : tags) {
    if (!first) {
      url.append("+");
    }

    std::string clean_tag = tag;
    std::replace(clean_tag.begin(), clean_tag.end(), ' ', '_');

    url.append(clean_tag);
    first = false;
  }

  url.append("&limit=" + std::to_string(limit));
  return url;
}

std::string KonaChan::getJsonFromUrl(const std::string &url) {
  try {
    return httpGet(url);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}
