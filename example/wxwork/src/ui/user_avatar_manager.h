#pragma once

#include <map>
#include <memory>
#include <zaf/graphic/image/image_source.h>
#include "entity/id.h"

class UserAvatarManager {
public:
    static UserAvatarManager& GetInstance();

public:
    UserAvatarManager() { }
    UserAvatarManager(const UserAvatarManager&) = delete;
    UserAvatarManager& operator=(const UserAvatarManager&) = delete;

    zaf::ImageSource GetUserAvatarImage(Id user_id);

private:
    std::map<Id, zaf::ImageSource> user_avatar_images_;
};
