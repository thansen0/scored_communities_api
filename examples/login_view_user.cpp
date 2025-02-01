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
    json user = a.getUserAuth();

    // prints out all json data
    cout << user << endl;

    // user will be empty if none exists
    if (user != nullptr && !user.empty()) {
        if (user.value("is_suspended", false)) {
            std::cout << "This user is suspended." << std::endl;
        }

        std::cout << "A list of the first ten communities user " << user.value("username", "") << " is subscribed to:" << std::endl;

        int i = 0;
        for (nlohmann::json com : user["subscribed"]) {
            std::cout << ++i << ": " << com << std::endl;

            if (i >= 10)
                break;
        }
    }

}
