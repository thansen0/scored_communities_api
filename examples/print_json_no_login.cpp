#include <vector>
#include <iostream>
#include <utility> // std::pair

#include "scoredapi.hpp"
#include "json.hpp"

using namespace scoredapi;

int main() {
    vector<nlohmann::json> page1 = ScoredCoApi::getFeedPublic(TRENDING, SORT_HOT, false);

    if (page1.size() > 0) {
        // print page
        std::cout << "About post, title: " << page1[0].value("title", "No Title") << ", post by " << page1[0].value("author", "") << " with a score of " << page1[0].value("score", 0) << std::endl;
        std::cout << "JSON post blob: " << page1[0] << std::endl;

        // get comment
        std::pair<nlohmann::json, std::vector<nlohmann::json>> p = ScoredCoApi::getPostPublic(page1[0]["id"]);

        // print first comment
        std::vector<nlohmann::json> comments = p.second;

        if (comments.size() > 0)
            std::cout << "JSON comment blob: " << comments[0] << std::endl;
        else
            std::cout << "No comments in first post" << std::endl;
    } else {
        std::cout << "No content found, not printing post or comment." << std::endl;
    }

}

