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
    std::string raw = httpGet(url);

    HttpResponse res = parseResponse(raw);
    if (getStatusCode(res.status) != 200) {
      return "";
    }

    std::string data = res.body;
    std::string te = getHeader(res.head, "Transfer-Encoding");

    if (te.find("chunked") != std::string::npos) {
      data = decodeChunked(data);
    }

    return data;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return "";
}

std::vector<PostData> KonaChan::parseJsonByString(const std::string &json_str) {
  std::vector<PostData> result;

  if (json_str.empty()) {
    std::cerr << "Json string is empty." << std::endl;
    return result;
  }

  try {
    auto arr = nlohmann::json::parse(json_str);

    result.reserve(arr.size());

    for (const auto &item : arr) {
      PostData post;

      post.id = item.value("id", 0);
      post.file_url = item.value("file_url", "");
      post.tags = item.value("tags", "");
      post.width = item.value("width", 0);
      post.height = item.value("height", 0);
      post.ext = item.value("file_ext", "");

      if (!post.file_url.empty())
        result.push_back(post);
    }

  } catch (const nlohmann::json::parse_error &e) {
    std::cerr << "JSON Parse Error: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error while parsing JSON: " << e.what() << "\n";
  }

  return result;
}
