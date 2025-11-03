#include <vector>
#include <iostream>

#include "scoredapi.hpp"
#include "json.hpp"

using namespace scoredapi;

int main() {
    vector<nlohmann::json> page1 = ScoredCoApi::getFeedPublic("funny", SORT_HOT, false);

    int i = 0;
    for (nlohmann::json post : page1) {
        std::cout << ++i << ": " << post.value("title", "") << endl;
    }

    vector<nlohmann::json> page2 = ScoredCoApi::getFeedPublic("funny", SORT_HOT, false, page1.back().value("uuid", ""));

    for (nlohmann::json post : page2) {
        std::cout << ++i << ": " << post.value("title", "") << endl;
    }

    // you could also append page2 onto page1 with
    // page1.insert(page1.end(), page2.begin(), page2.end());
}
