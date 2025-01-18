
#ifndef INCLUDE_SCORED_COMMUNITIES_API_HPP_
#define INCLUDE_SCORED_COMMUNITIES_API_HPP_

// From nlohmann/json
#include "json.hpp"
#include <iostream>
#include <vector>
#include <utility> // std::pair
#include <curl/curl.h>
#include <thread>
#include <set>

// All the different sort options for feeds
// https://docs.scored.co/api/feeds/getting-started#sort-options
#define HOT             "hot"
#define NEW             "new"
#define ACTIVE          "active"
#define RISING          "rising"
#define TOP             "top"

// Comment sort methods
// #define TOP             "top"
#define CONTROVERSIAL   "controversial"
// #define NEW             "new"
#define OLD             "old"


#define TRENDING        "win&isTrendingTopics=true&trendingTopics=%5B%5D"
#define HOME            "Home"

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

class ScoredCoApi {
private:
    std::string APIKey;
    vector<nlohmann::json> posts;

    //inline static const std::set<string> validCommentSorts = {HOT, NEW, ACTIVE, RISING, TOP};
    // inline static const std::set<string> validPostSorts = {NEW, TOP, CONTROVERSIAL, OLD};

    //static const std::set<std::string>& getValidCommentSorts() {
    //    return validCommentSorts;
    //}

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

    static vector<nlohmann::json> getFeed(const std::string community=TRENDING, const std::string sort=HOT, const bool appSafe=false, const std::string post_uuid="") {

        std::vector<nlohmann::json> scored_feed;
        std::string base_url = "https://api.scored.co/api/v2/post/" + sort + "v2.json?community=" + community;

        // sanatize sort input
        // if (validCommentSorts.find(sort) == validCommentSorts.end()) {
        if (!(HOT == sort || NEW == sort || ACTIVE == sort || RISING == sort || TOP == sort)) {
            cerr << "\"" << sort << "\" value not valid for feed sorting. Returning empty vector";
            return scored_feed;
        }

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
                    // add json comment to post vector
                    if (post_uuid != post.value("uuid", "")) {
                        // won't add the initial post if paginating
                        // also won't add post if no pagination uuid is passed in, and
                        // json is malformed (incidental behavior)
                        scored_feed.push_back(post);
                    }
                }
            }
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }

        return scored_feed;
    }

    static std::pair<nlohmann::json, std::vector<nlohmann::json>> getPost(const unsigned int post_id, const bool get_comments=true, const std::string commentSort=TOP) {
        std::string base_url = "https://api.scored.co/api/v2/post/post.json?id=" + std::to_string(post_id);

        nlohmann::json post;
        std::vector<nlohmann::json> comments;

        if (get_comments) {
            // post with no comments
            base_url += "&comments=true";
        }

        if (commentSort == CONTROVERSIAL || commentSort == NEW || commentSort == OLD) {
            // will use default commentSort method if invalid value is entered
            // different from post sort, where it's required for the query and fails
            // if an invalid value is entered
            base_url += "&commentSort=" + commentSort;
        }

        cout << base_url << endl << endl;

        string jsonDataStr = ScoredCoApi::GETRequest(base_url);
        
        try {
            nlohmann::json all_json_data = nlohmann::json::parse(jsonDataStr);

            if (all_json_data.contains("posts")) {
                // posts is plural but there should only be one
                post = all_json_data["posts"][0];
            }

            if (all_json_data.contains("comments") && all_json_data["comments"].is_array()) {
                const auto& json_comments = all_json_data["comments"];

                for (const auto& comment : json_comments) {
                    // add to post comment vector
                    comments.push_back(comment);
                }
            }
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }


        return std::make_pair(post, comments);
    }
};


#endif // INCLUDE_SCORED_COMMUNITIES_API_HPP_
