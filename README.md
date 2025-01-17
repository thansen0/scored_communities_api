# Scored Communities C++ API

This is a C++ implementation of the [scored.co](https://scored.co/) API. This is the same site as patriots.win and communities.win, and you can use their API interchangeably.

## Building

This project (unfortunately) has two dependencies. json.hpp for parsing json (nlohmann/json), and libcurl for processing GET and POST requests. Be sure to include `-lcurl` when building with g++, or to put it in your CMake file.

# Usage

## Static Library (no user sign-in)

You can use this library statically, without signing in. An example might look like the following.

```
#include "scoredapi.hpp"
#include "json.hpp"

int main() {
    vector<nlohmann::json> page1 = ScoredCoApi::getFeed("funny", HOT, false);

    int i = 0;
    for (nlohmann::json post : page1) {
        std::cout << ++i << ": " << post.value("title", "") << endl;

        // print post comments
        if (i == 1) {
            // we print all the comments from the 1st post
            std::pair<nlohmann::json, std::vector<nlohmann::json>> p = ScoredCoApi::getPost(post["id"]);

            // p.first is post data (same as post), p.second is comment vector
            vector<nlohmann::json> comments = p.second;
            for (auto c : comments) {
                cout << c.value("id", 0) << ": " << c.value("raw_content", "") << endl;;
            }
        }
    }

    vector<nlohmann::json> page2 = ScoredCoApi::getFeed("funny", HOT, false, test.back().value("uuid", ""));

    for (nlohmann::json post : page2) {
        std::cout << ++i << ": " << post.value("title", "") << endl;
    }
}
```
