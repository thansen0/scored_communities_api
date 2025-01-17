
#ifndef INCLUDE_SCORED_COMMUNITIES_API_HPP_
#define INCLUDE_SCORED_COMMUNITIES_API_HPP_

// From nlohmann/json
#include "json.hpp"
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <thread>

// All the different sort options
// https://docs.scored.co/api/feeds/getting-started#sort-options
#define HOT             "hot"
#define NEW             "new"
#define ACTIVE          "active"
#define RISING          "rising"
#define TOP             "top"

#define TRENDING        "win&isTrendingTopics=true&trendingTopics=%5B%5D"
#define HOME            "Home"

using namespace std;

struct ScoredPost {
    bool is_stickied; // 1 byte
    bool is_nsfw;
    bool is_locked;
    bool is_twitter;
    bool is_image;
    bool is_crosspost;
    int comments;
    unsigned int score;
    std::string title;
    std::string content;
    std::string author;
    std::string preview;
    std::string url;
    std::string uuid;
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
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
            // curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
            // curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
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

    static vector<struct ScoredPost> getFeed(const std::string community=TRENDING, const std::string sort=HOT, const bool appSafe=false, const std::string post_uuid="") {

        std::vector<struct ScoredPost> scored_feed;
        std::string base_url = "https://api.scored.co/api/v2/post/" + sort + "v2.json?community=" + community;

        if (!post_uuid.empty()) {
            // set pagination if not on first page
            base_url += "&from=" + post_uuid;
        }

        if (appSafe) {
            // only allow sfw content
            base_url += "&appSafe=true";
        }

        cout << base_url << endl << endl;

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
                    cur_post.is_image = post.value("is_image", false);
                    cur_post.is_crosspost = post.value("is_crosspost", false);
                    cur_post.comments = post.value("comments", 0);
                    cur_post.score = post.value("score", 0);
                    cur_post.title = post.value("title", "");
                    cur_post.content = post.value("content", "");
                    cur_post.author = post.value("author", "");
                    cur_post.preview = post.value("preview", "");
                    cur_post.url = post.value("url", "");
                    cur_post.uuid = post.value("uuid", "");

                    scored_feed.push_back(cur_post);
                }
            }
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }

        return scored_feed;
    }
};



int main() {
    vector<struct ScoredPost> test = ScoredCoApi::getFeed("funny", /* sort= */ HOT, /* appSafe= */ false, "");

    int i = 0;
    for (ScoredPost post : test) {
        std::cout << ++i << ": " << post.title << endl;
    }

    // std::this_thread::sleep_for(std::chrono::seconds(5));

    vector<struct ScoredPost> test2 = ScoredCoApi::getFeed("funny", /* sort= */ HOT, /* appSafe= */ false, test[24].uuid);

    for (ScoredPost post : test2) {
        std::cout << ++i << ": " << post.title << endl;
    }

}

#endif // INCLUDE_SCORED_COMMUNITIES_API_HPP_
