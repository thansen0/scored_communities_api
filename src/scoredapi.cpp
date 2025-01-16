
#ifndef INCLUDE_SCORED_COMMUNITIES_API_HPP_
#define INCLUDE_SCORED_COMMUNITIES_API_HPP_

// From nlohmann/json
#include "json.hpp"
#include <iostream>
#include <vector>
#include <curl/curl.h>

// All the different sort options
// https://docs.scored.co/api/feeds/getting-started#sort-options
#define HOT             "hot"
#define NEW             "new"
#define ACTIVE          "active"
#define RISING          "rising"
#define top             "top"

using namespace std;

struct ScoredPost {
    bool is_stickied; // 1 byte
    bool is_nsfw;
    bool is_locked;
    bool is_twitter;
    int comments;
    unsigned int score;
    std::string title;
    std::string author;
    std::string preview;
    std::string url;
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string GETRequest2(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize CURL." << std::endl;
    }
    
    return response;
}

class ScoredCoApi {
private:
    std::string APIKey;
    vector<ScoredPost> posts;

    static std::string GETRequest(const std::string& url) {
        CURL* curl = curl_easy_init();
        std::string response;

        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            }

            // Clean up
            curl_easy_cleanup(curl);
        } else {
            std::cerr << "Failed to initialize CURL." << std::endl;
        }

        return response;
    }

public:
    ScoredCoApi(std::string username, std::string password) {
        // fetch API key
    }

    static vector<struct ScoredPost> getFeed(std::string community, std::string sort=HOT) {
        std::vector<struct ScoredPost> scored_feed;
        //std::string base_url = "https://api.scored.co/api/v2/post/hotv2.json?community=meta";
        // std::string base_url = format("https://api.scored.co/api/v2/post/{}v2.json?community={}", sort, community);
        std::string base_url = "https://api.scored.co/api/v2/post/" + sort + "v2.json?community=" + community;
        
        cout << base_url << endl;

        string jsonDataStr = ScoredCoApi::GETRequest(base_url);

        try {
            auto all_json_data = nlohmann::json::parse(jsonDataStr);

            if (all_json_data.contains("posts") && all_json_data["posts"].is_array()) {
                const auto& json_posts = all_json_data["posts"];

                for (const auto& post : json_posts) {
                    // build post, you can easily add or remove values
                    // from the struct here if you'd like
                    ScoredPost cur_post;

                    cur_post.is_stickied = post.value("is_stickied", false);
                    cur_post.is_nsfw = post.value("is_nsfw", false);
                    cur_post.is_locked = post.value("is_locked", false);
                    cur_post.is_twitter = post.value("is_twitter", false);
                    cur_post.comments = post.value("comments", 0);
                    cur_post.score = post.value("score", 0);
                    cur_post.title = post.value("title", "");
                    cur_post.author = post.value("author", "");
                    cur_post.preview = post.value("preview", "");
                    cur_post.url = post.value("url", "");

                    scored_feed.push_back(cur_post);
                }
            }
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }

        //cout << jsonDataStr << endl;

        return scored_feed;
    }
};



int main() {
    vector<struct ScoredPost> test = ScoredCoApi::getFeed("meta");

    for (ScoredPost post : test) {
        std::cout << post.title << endl;
    }
}

#endif // INCLUDE_SCORED_COMMUNITIES_API_HPP_
