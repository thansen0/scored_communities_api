
#ifndef INCLUDE_SCORED_COMMUNITIES_API_HPP_
#define INCLUDE_SCORED_COMMUNITIES_API_HPP_

// From nlohmann/json
#include "json.hpp"
#include <iostream>
#include <vector>
#include <curl/curl.h>

#define HOT             "hot"
#define NEW             "new"

using namespace std;

struct ScoredPost {
    std::string title;
    std::string preview;
    std::string url;
    bool is_stickied;
    bool is_nsfw;
    int comments;
    unsigned int score;
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
                    // 
                    std::cout << "title: " << post["title"] << std::endl;
                }
            }
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }

        //cout << jsonDataStr << endl;

        std::vector<struct ScoredPost> aa;
        return aa;
    }
};



int main() {
    cout << NEW << endl;

    ScoredCoApi::getFeed("meta");
}

#endif // INCLUDE_SCORED_COMMUNITIES_API_HPP_
