//                             _            
//                            | |           
//  ___  ___ ___  _ __ ___  __| |  ___ ___  
// / __|/ __/ _ \| '__/ _ \/ _` | / __/ _ \     C++ Library for scored.co
// \__ \ (_| (_) | | |  __/ (_| || (_| (_) |    version 0.0.1
// |___/\___\___/|_|  \___|\__,_(_)___\___/     https://github.com/thansen0
//                                          
// 
// SPDX-FileCopyrightText: 2025 thansen0 <https://github.com/thansen0>
// SPDX-License-Identifier: Unlicense

#ifndef INCLUDE_SCORED_COMMUNITIES_API_HPP_
#define INCLUDE_SCORED_COMMUNITIES_API_HPP_

// From nlohmann/json
#include "json.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <utility> // std::pair
#include <curl/curl.h>
// #include <thread>
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

#define DEBUG           false

using namespace std;

using HeaderMap = std::map<std::string, std::vector<std::string>>;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
    size_t totalSize = size * nitems;
    HeaderMap* headers = static_cast<HeaderMap*>(userdata);

    std::string headerLine(buffer, totalSize);

    // Find the separator between header name and value
    size_t separator = headerLine.find(':');
    if (separator != std::string::npos) {
        std::string headerName = headerLine.substr(0, separator);
        // Remove potential leading/trailing whitespace and the colon
        std::string headerValue = headerLine.substr(separator + 1);
        // Trim whitespace
        headerName.erase(headerName.find_last_not_of(" \t\r\n") + 1);
        headerValue.erase(0, headerValue.find_first_not_of(" \t\r\n"));
        headerValue.erase(headerValue.find_last_not_of(" \t\r\n") + 1);

        // Insert into the map (convert header names to lowercase for case-insensitive access)
        std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::tolower);
        headers->emplace(headerName, std::vector<std::string>()).first->second.push_back(headerValue);
    }

    return totalSize;
}

/**
 * @brief Contains all of the logic for interacting with the scored.co API
 *
 * The ScoredCoApi interacts with the scored.co API v2, and uses static
 * methods for interacting with the API when a user is signed in, or 
 * can be instantiated and used with a signed-in user.
 */
class ScoredCoApi {
private:
    std::string public_key, private_key;
    // vector<nlohmann::json> posts;

    // static const std::set<string> validCommentSorts = {HOT, NEW, ACTIVE, RISING, TOP};
    // static const std::set<string> validPostSorts = {NEW, TOP, CONTROVERSIAL, OLD};


    /**
     * @brief Performs a GET request using libcurl.
     * 
     * @param url The complete URL to perform the GET request on.
     *
     * @return A string of the response, or an empty string in the case of an error.
     */
    static std::string GETRequest(const std::string& url) {
        CURL* curl = curl_easy_init();
        std::string response;

        if (!curl) {
            std::cerr << "Failed to init curl\n";
            return response;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        // curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        } else {
            if (DEBUG) {
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

                std::cout << "GETRequest HTTP Response Code: " << http_code << std::endl;
            }
        }

        // Clean up
        curl_easy_cleanup(curl);

        return response;
    }

    /**
     * @brief Performs a GET request using libcurl, and passes through api keys.
     * 
     * @param url The complete URL to perform the GET request on.
     *
     * @return A string of the response, or an empty string in the case of an error.
     */
    std::string GETRequestAuth(const std::string& url) {
        CURL* curl = curl_easy_init();

        struct curl_slist* headers = nullptr;
        std::string response;

        if (this->public_key == "" || this->private_key == "") {
            cerr << "User key not present" << endl;
            return response;
        } 

        if (!curl) {
            std::cerr << "Failed to init curl\n";
            return response;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        // curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // add api key headers
        headers = curl_slist_append(headers,
                string("x-api-key: ").append(this->public_key).c_str()
        );
        headers = curl_slist_append(headers,
                string("x-api-secret: ").append(this->private_key).c_str()
        );

        // attach header to curl object 
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            return response;
        } else {
            if (DEBUG) {
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

                std::cout << "GETRequestAuth HTTP Response Code: " << http_code << std::endl;
            }
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return response;
    }

    /**
     * @brief Performs a POST request using libcurl, and passes through api keys.
     * 
     * @param url The complete URL to perform the POST request on.
     *
     * @return A string of the response, or an empty string in the case of an error.
     */
    std::string POSTRequestAuth(const std::string& url, const std::string& parameters) {
        std::string response;
        struct curl_slist* headers = nullptr;

        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to init curl\n";
            return response;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // set for POST request
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, parameters.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) parameters.length());

        // add api key headers
        headers = curl_slist_append(headers,
                string("x-api-key: ").append(this->public_key).c_str()
        );
        headers = curl_slist_append(headers,
                string("x-api-secret: ").append(this->private_key).c_str()
        );

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << '\n';
        } else {
            if (DEBUG) {
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

                std::cout << "POSTRequestAuth HTTP Response Code: " << http_code << std::endl;
            }
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return response;
    }

    /**
     * @brief Performs a POST request to access the LTS token cookie, and returnit.
     * 
     * @param url The complete URL to perform the POST request on.
     * @param parameteres .
     *
     * @return A string containing just the token, or an empty string in the case of an error.
     */
    std::string POSTRequestForLTSCookie(const std::string parameters) {
        CURL* curl = curl_easy_init();
        std::string response;
        HeaderMap responseHeaders;

        char LTS_cookie[50]; // I think this could just be 44 but rounded up

        if (!curl) {
            std::cerr << "Failed to init curl\n";
            return response;
        }

        CURLcode res;

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.scored.co/api/v2/user/login");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, parameters.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded"); // "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up the callback to receive the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // 5 seconds
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);


        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " 
                      << curl_easy_strerror(res) << std::endl;
        } else {
            if (DEBUG) {
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

                std::cout << "POSTRequestForLTSCookie HTTP Response Code: " << http_code << std::endl;
            }

            // std::cout << "Response Body: " << response << std::endl;

            auto it = responseHeaders.find("set-cookie");
            if(it != responseHeaders.end()) {
                for(const std::string& cookie : it->second) {
                    // std::cout << "LTS header: " << cookie << std::endl;
                    if (cookie[0] == 'L' && cookie[1] == 'T' && cookie[2] == 'S') {
                        size_t LTS_token_end = cookie.find(';') - 4;
                        std::memcpy(LTS_cookie, cookie.c_str() + 4, LTS_token_end);

                        LTS_cookie[LTS_token_end+1] = '\0';
                        break;
                    }
                }
            } else {
                std::cerr << "Set-Cookie Header not found." << std::endl;
            }
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return std::string(LTS_cookie);
    }

    /**
     * @brief Performs a POST request to access the public and private keys.
     * 
     * @param url The complete URL to perform the POST request on.
     * @param LTS token
     *
     * @return JSON string containing public and private keys
     */
    std::string POSTRequestForKeys(std::string lts_token) {
        std::string response;

        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Failed to init curl\n";
            return response;
        }

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.scored.co/api/v2/token");

        // We want a POST request
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // 5 seconds

        // provide the cookie header string
        // e.g. "LTS=XYZ; XSRF-TOKEN=ABC; WSID01=DEF"
        lts_token = "LTS=" + lts_token;
        curl_easy_setopt(curl, CURLOPT_COOKIE, lts_token.c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << '\n';
        } else {
            if (DEBUG) {
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

                std::cout << "POSTRequestForKeys HTTP Response Code: " << http_code << std::endl;
            }
        }

        curl_easy_cleanup(curl);

        return response;
    }

public:
    ScoredCoApi(std::string username="", std::string password="") {
        if (username != "" && password != "") {
            this->setUsernamePassword(username, password);
        }
    }

    void setUsernamePassword(std::string username, std::string password) {
        // fetch API key
        std::string url = "https://api.scored.co/api/v2/user/login";
        std::string parameters = "username=" + username + "&password=" + password;

        std::string LTS_token = POSTRequestForLTSCookie(parameters);

        // cout << LTS_token << endl;

        std::string jsonDataStr = POSTRequestForKeys(LTS_token);

        // cout << jsonDataStr << endl;

        try {
            auto all_json_data = nlohmann::json::parse(jsonDataStr);

            if (all_json_data.contains("status") && all_json_data["status"]) {
                this->public_key = all_json_data["api_key"];
                this->private_key = all_json_data["api_secret"];
            } else {
                // return or print out some error
                cerr << "Error; not logged in." << all_json_data.value("error", "") << endl;
            }

        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }

        // printf("pub_key:  %s \npriv_key: %s", this->public_key, this->private_key);
    }

    void setApiKeys(std::string public_key, std::string private_key) {
        this->public_key = public_key;
        this->private_key = private_key;
    }

    /**
     * @brief Gets the feed from a specific community.
     * 
     * @param community The community you would like to read from.
     * @param sort      Sorting method, choose from HOT, NEW, ACTIVE, RISING, or TOP.
     * @param appSafe   Whether the community is safe for the app store.
     * @param post_uuid The post UUID from which you want to start paginating. Used 
     *                  for multiple page requests 
     *
     * @return A JSON object containing relevant data.
     */
    static vector<nlohmann::json> getFeedPublic(const std::string community=TRENDING, const std::string sort=HOT, const bool appSafe=false, const std::string post_uuid="") {

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

        // cout << base_url << endl << endl;

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

    /**
     * @brief Gets the feed from a specific community from an authenticated user.
     * 
     * @param community The community you would like to read from.
     * @param sort      Sorting method, choose from HOT, NEW, ACTIVE, RISING, or TOP.
     * @param appSafe   Whether the community is safe for the app store.
     * @param post_uuid The post UUID from which you want to start paginating. Used 
     *                  for multiple page requests 
     *
     * @return A JSON object containing relevant data.
     */
    vector<nlohmann::json> getFeed(const std::string community=TRENDING, const std::string sort=HOT, const bool appSafe=false, const std::string post_uuid="") {

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

        // cout << base_url << endl << endl;

        string jsonDataStr = this->GETRequestAuth(base_url);

        if (jsonDataStr == "") {
            std::cerr << "GET Request failed; possibly not signed in or not connected to the internet" << std::endl;
            return scored_feed;
        }

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

    /**
     * @brief Gets the comments and information from a specific post.
     * 
     * @param post_id       ID from which the post belongs.
     * @param get_comments  Boolean value of whether to retrieve comments or not.
     * @param commentSort   Comment sort direction (default TOP, optionally
     *                      CONTROVERSIAL, NEW, and OLD. 
     *
     * @return A std::pair, first of the post, second containing a vector of comments.
     */
    static std::pair<nlohmann::json, std::vector<nlohmann::json>> getPostPublic(const unsigned int post_id, const bool get_comments=true, const std::string commentSort=TOP) {
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

        // cout << base_url << endl << endl;

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

    /**
     * @brief Gets the comments and information from a specific post from an authenticated user.
     * 
     * @param post_id       ID from which the post belongs.
     * @param get_comments  Boolean value of whether to retrieve comments or not.
     * @param commentSort   Comment sort direction (default TOP, optionally
     *                      CONTROVERSIAL, NEW, and OLD. 
     *
     * @return A std::pair, first of the post, second containing a vector of comments.
     */
    std::pair<nlohmann::json, std::vector<nlohmann::json>> getPost(const unsigned int post_id, const bool get_comments=true, const std::string commentSort=TOP) {
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

        // cout << base_url << endl << endl;

        string jsonDataStr = this->GETRequestAuth(base_url);
        
        if (jsonDataStr == "") {
            std::cerr << "GET Request failed; possibly not signed in or not connected to the internet" << std::endl;
            return std::make_pair(post, comments);
        }

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

    /**
     * @brief Retrieves specific user.
     *
     * @param username User's username.
     * 
     * @return json object containing the specified user's information.
     */
    static nlohmann::json getUserPublic(const std::string username) {
        nlohmann::json user_data;

        std::string base_url = "https://api.scored.co/api/v2/user/about.json?user=" + username;

        // cout << base_url << endl << endl;

        string jsonDataStr = ScoredCoApi::GETRequest(base_url);

        try {
            nlohmann::json all_json_data = nlohmann::json::parse(jsonDataStr);

            if (!all_json_data.value("status", false)) {
                // GET request has failed, return empty json
                std::cerr << "Error finding user" << std::endl;
                return user_data;
            }

            if (all_json_data.contains("users") && all_json_data["users"].size() > 0) {
                // should be an array of one user (i.e. the user we want)
                user_data = all_json_data["users"][0];
            }

        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }

        return user_data;
    }

    /**
     * @brief Retrieves specific user data from a signed in user.
     *
     * @param username User's username.
     * 
     * @return json object containing the specified user's information.
     */
    nlohmann::json getUser(const std::string username="me") {
        nlohmann::json user_data;

        std::string base_url = "https://api.scored.co/api/v2/user/about.json?user=" + username;

        string jsonDataStr = this->GETRequestAuth(base_url);

        if (jsonDataStr == "") {
            std::cerr << "GET Request failed; possibly not signed in or not connected to the internet" << std::endl;
            return user_data;
        }

        try {
            nlohmann::json all_json_data = nlohmann::json::parse(jsonDataStr);

            if (!all_json_data.value("status", false)) {
                // GET request has failed, return empty json
                std::cerr << "Error finding user" << std::endl;
                return user_data;
            }

            if (all_json_data.contains("users") && all_json_data["users"].size() > 0) {
                // should be an array of one user (i.e. the user we want)
                user_data = all_json_data["users"][0];
            }

        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }

        return user_data;
    }

    nlohmann::json sendVote(const int post_id, const bool isPost, const bool voteDirection) {
        nlohmann::json vote_data;
        string url = "https://api.scored.co/api/v2/action/vote";

        stringstream ss;
        ss << "id=" << post_id
                << "&type=" << (isPost ? "true" : "false")
                << "&direction=" << (voteDirection ? "true" : "false");
        string parameters = ss.str();

        string jsonDataStr = POSTRequestAuth(url, parameters);
        cout << jsonDataStr << endl << endl;

        if (jsonDataStr == "") {
            std::cerr << "GET Request failed; possibly not signed in or not connected to the internet" << std::endl;
            return vote_data;
        }

        try {
            nlohmann::json all_json_data = nlohmann::json::parse(jsonDataStr);

            if (!all_json_data.value("status", false)) {
                // POST request has failed, return empty json
                std::cerr << "Error finding user" << std::endl;
                return vote_data;
            }

            /*if (all_json_data.contains("users") && all_json_data["users"].size() > 0) {
                // should be an array of one user (i.e. the user we want)
                vote_data = all_json_data["users"][0];
            }*/

        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        } catch (const nlohmann::json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << std::endl;
        }

        return vote_data;
    }

};


#endif // INCLUDE_SCORED_COMMUNITIES_API_HPP_
