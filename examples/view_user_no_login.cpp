#include <vector>
#include <iostream>
#include <utility> // std::pair

#include "scoredapi.hpp"
#include "json.hpp"

int main() {
    // we will get the Admin user C
    nlohmann::json user = ScoredCoApi::getUserPublic("c");

    // view json blob in terminal 
    // std::cout << user << std::endl;

    // user will be empty if none exists
    if (user != nullptr && !user.empty()) {
        if (user.value("is_suspended", false)) {
            std::cout << "This user is suspended." << std::endl;
        }

        std::cout << "A list of the first ten communities user " << user.value("username", "") << " moderates:" << std::endl;

        int i = 0;
        for (nlohmann::json com : user["moderates"]) {
            std::cout << ++i << ": " << com << std::endl;

            if (i >= 10)
                break;
        }
    }

}

