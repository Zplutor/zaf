#pragma once

#include <map>
#include <memory>
#include <zaf/graphic/wic/bitmap_source.h>
#include "entity/id.h"

class UserAvatarManager {
public:
    static UserAvatarManager& GetInstance();

public:
    UserAvatarManager() { }
    UserAvatarManager(const UserAvatarManager&) = delete;
    UserAvatarManager& operator=(const UserAvatarManager&) = delete;

    zaf::wic::BitmapSource GetUserAvatarImage(Id user_id);

private:
    std::map<Id, zaf::wic::BitmapSource> user_avatar_images_;
};
