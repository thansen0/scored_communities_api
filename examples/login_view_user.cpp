#include <stdio.h>
#include <iostream>

#include "scoredapi.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    string username, password;

    if (argc == 3) {
        /*
        std::cerr << "You must enter a username and password into the command line.\n\n" \
                  << "./login_view_user username password" \
                  << std::endl;*/
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

    cout << username << ":" << password << endl;

    ScoredCoApi a(username, password);

}
