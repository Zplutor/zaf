#pragma once

#include <map>
#include <memory>
#include <vector>
#include "entity/user.h"

class UserStorage {
public:
    UserStorage() { }
    UserStorage(const UserStorage&) { }
    UserStorage& operator=(const UserStorage&) { }

    std::vector<std::shared_ptr<User>> GetUsers(const std::vector<Id>& user_ids);
    std::vector<Id> AddUsers(const std::vector<std::shared_ptr<User>>& users);

private:
    std::map<Id, std::shared_ptr<User>> users_;
};