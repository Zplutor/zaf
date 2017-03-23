#include "storage/user_storage.h"
#include "storage/id_generating.h"

std::vector<std::shared_ptr<User>> UserStorage::GetUsers(const std::vector<Id>& user_ids) {

    std::vector<std::shared_ptr<User>> users;

    for (auto each_id : user_ids) {

        auto iterator = users_.find(each_id);
        if (iterator != users_.end()) {
            auto user = std::make_shared<User>(*(iterator->second));
            users.push_back(user);
        }
    }

    return users;
}


std::vector<Id> UserStorage::AddUsers(const std::vector<std::shared_ptr<User>>& users) {

    std::vector<Id> added_ids;

    for (const auto& each_user : users) {

        auto new_user = std::make_shared<User>(*each_user);
        new_user->id = GenerateId();

        users_.insert(std::make_pair(new_user->id, new_user));
        added_ids.push_back(new_user->id);
    }

    return added_ids;
}

