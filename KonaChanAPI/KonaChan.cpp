#include "KonaChan.h"

// for test
#include <iostream>

const std::string KonaChan::base_url_ = "https://konachan.net/post.json?";

void KonaChan::downloadByTagsLimit(const std::vector<std::string> &tags,
                                   int limit) {
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
  std::string res = std::move(httpGet(url));

  std::cout << url << std::endl;
  std::cout << res << std::endl;
}
