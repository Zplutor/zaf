#include "logic/data_generating/user_generating.h"

std::vector<std::shared_ptr<User>> GenerateUsers() {

    static const wchar_t* const male_names[] = {
        L"Jack",
        L"Thomas",
        L"Joshua",
        L"William",
        L"Daniel",
        L"Matthew",
        L"James",
        L"Joseph",
        L"Harry",
        L"Samuel",
    };

    static const wchar_t* const female_names[] = {
        L"Emily",
        L"Chloe",
        L"Megan",
        L"Jessica",
        L"Emma",
        L"Sarah",
        L"Elizabeth",
        L"Sophie",
        L"Olivia",
        L"Lauren",
    };

    std::vector<std::shared_ptr<User>> users;

    for (auto each_name : male_names) {
        auto user = std::make_shared<User>();
        user->name = each_name;
        user->gender = User::Gender::Male;
        users.push_back(user);
    }

    for (auto each_name : female_names) {
        auto user = std::make_shared<User>();
        user->name = each_name;
        user->gender = User::Gender::Female;
        users.push_back(user);
    }

    return users;
}