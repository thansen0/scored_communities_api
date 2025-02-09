# Scored Communities C++ API

This is a C++ implementation of the [scored.co](https://scored.co/) API. This is the same site as patriots.win and communities.win, and you can use their API interchangeably. This library is **not** officially affiliated with scored.co, patriots.win, or any of their associated sites.

## Building

This project (unfortunately) has two dependencies. json.hpp for parsing json (nlohmann/json), and libcurl for processing GET and POST requests. Be sure to include `-lcurl` when building with g++, or to put it in your CMake file. When using this project, you **must** include both `json.hpp` ([json.hpp](src/json.hpp)) and `scoredapi.hpp` ([scoredapi.hpp](src/scoredapi.hpp)) in the include or src folder in your project.

If you would like to build this project and run the examples, you may run

```
mkdir build
cd build
cmake ../
make
```

# Usage

## Static Library (no user sign-in)

You can use this library statically, without signing in or creating a class instance.

Here we access one page, and print out the comments from the first post while printing out the title of each post. Then we access a second page, and print out all the titles of those posts. Pages usually render in sets of 25 posts. Since we're not signed in, we're only able to access public data.

```C++
#include "scoredapi.hpp"
#include "json.hpp"

int main() {
    vector<nlohmann::json> page1 = ScoredCoApi::getFeedPublic("funny", HOT, false);

    int i = 0;
    for (nlohmann::json post : page1) {
        std::cout << ++i << ": " << post.value("title", "") << endl;

        // print post comments
        if (i == 1) {
            // we print all the comments from the 1st post
            std::pair<nlohmann::json, std::vector<nlohmann::json>> p = ScoredCoApi::getPostPublic(post["id"]);

            // p.first is post data (same as post), p.second is comment vector
            vector<nlohmann::json> comments = p.second;
            for (auto c : comments) {
                cout << c.value("id", 0) << ": " << c.value("raw_content", "") << endl;;
            }
        }
    }

    vector<nlohmann::json> page2 = ScoredCoApi::getFeedPublic("funny", HOT, false, test.back().value("uuid", ""));

    for (nlohmann::json post : page2) {
        std::cout << ++i << ": " << post.value("title", "") << endl;
    }
}
```

Here we are limited to only viewing feeds of posts and comments.

Posts (and comments) are returned as json objects, courtesy of `nlohmann/json`. You can view all fields by printout out the object (i.e. `cout << post << endl`), and you can access the fields though square brackets (i.e. `post["title"]`). Using square brackets will crash if the field doesn't exist, so you can either use the value function `post.value("title", "default")` to return the value or a default value, or you can use `post.contains("title")` to return a `bool` on whether or not the field exists.

## Library Sign-in

You can create an object and sign in by creating a `ScoredCoApi` object.

```C++
ScoredCoApi user(username, password);

vector<nlohmann::json> page1 = user.getFeed("funny", HOT, false);
```

You may also build a `FeedBuilder` object, which will automatically expand the feed as you read from it. Calling the hasNext() function checks for the feed size, and will expand the internal vector as required.

```C++
ScoredCoApi user(username, password);

ScoredCoApi::FeedBuilder fb = user.buildFeed(8);

while (fb.hasNext()) {
    nlohmann::json cur = fb.next();
    cout << cur.value("title", "") << endl;
}
```

# LICENSE

This library, and specifically the file [scoredapi.hpp](src/scoredapi.hpp), is open source, using the Unlicense. This effectively means it's in the public domain and you may relicense it with whatever license you choose.

[json.hpp](src/json.hpp) however, is a third party library by [nlohmann](https://github.com/nlohmann/json), and has the MIT license. It is open sourced, and you may use it, however you may not remove the MIT license notifications from `json.hpp`, while you can remove them in the [scoredapi.hpp](src/scoredapi.hpp) file.
