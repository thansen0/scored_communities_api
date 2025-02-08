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

    ScoredCoApi user(username, password);

    ScoredCoApi::FeedBuilder fb = user.buildFeed(8);

    cout << "Starting first iteration loop" << endl;
    while (fb.hasNext()) {
        json cur = fb.next();
        cout << cur.value("id", 1) << endl;
    }

    cout << endl << "Starting next iteration loop" << endl;
    while (fb.hasNext()) {
        json cur = fb.next();
        cout << cur.value("id", 1) << endl;
    }
}
