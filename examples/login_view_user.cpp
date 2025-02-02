#include <stdio.h>
#include <iostream>

#include "scoredapi.hpp"
#include "json.hpp"

using namespace std;
using namespace nlohmann;

int main(int argc, char *argv[]) {
    string username, password;

    if (argc == 3) {
        // use username and password from 
        username.assign(argv[1]);
        password.assign(argv[2]);
    } else {
        // use command line values
        std::cout << "username: ";
        std::cin >> username;

        std::cout << "password: ";
        std::cin >> password;
    }

    ScoredCoApi a(username, password);

    // by default, it grabs the signed in user. You may pass in
    // any username you like however
    json user = a.getUser();

    // prints out all json data
    cout << user << endl;

    // user will be empty if none exists
    if (user != nullptr && !user.empty()) {
        if (user.value("is_suspended", false)) {
            std::cout << "This user is suspended." << std::endl;
        }

        std::cout << "A list of the first ten communities user " << user.value("username", "") << " is subscribed to:" << std::endl << std::endl;

        int i = 0;
        for (nlohmann::json com : user["subscribed"]) {
            std::cout << ++i << ": " << com << std::endl;

            if (i >= 10)
                break;
        }
    }

    vector<json> feed = a.getFeed();

    if (feed.size() == 0) {
        cout << "Feed is empty" << endl;
        cout << feed << endl;
        return 1;
    }

    cout << feed[0] << endl;

    int post_id = feed[0].value("id", 0);

    pair<json, vector<json>> post = a.getPost(post_id);

    // print out post id I got above and post contents
    cout << "Post id: " << post_id << " has " << post.second.size() << " comments." << endl;
    // comments vector in post.second

}
