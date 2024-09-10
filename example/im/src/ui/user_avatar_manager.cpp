#include "ui/user_avatar_manager.h"
#include <Windows.h>
#include <zaf/application.h>
#include <zaf/graphic/wic/imaging_factory.h>
#include "resource.h"

static zaf::wic::BitmapSource CreateUserAvatarImage(Id user_id);
static HBITMAP GetUserAvatarData(Id user_id);

UserAvatarManager& UserAvatarManager::GetInstance() {
    static UserAvatarManager instance;
    return instance;
}


zaf::wic::BitmapSource UserAvatarManager::GetUserAvatarImage(Id user_id) {

    auto iterator = user_avatar_images_.find(user_id);
    if (iterator != user_avatar_images_.end()) {
        return iterator->second;
    }

    auto image = CreateUserAvatarImage(user_id);
    if (!image) {
        return zaf::wic::BitmapSource();
    }

    user_avatar_images_.insert(std::make_pair(user_id, image));
    return image;
}


static zaf::wic::BitmapSource CreateUserAvatarImage(Id user_id) {

    auto bitmap_handle = GetUserAvatarData(user_id);
    if (bitmap_handle == nullptr) {
        return zaf::wic::BitmapSource();
    }

    zaf::wic::BitmapCreateFromHBITMAPOptions options;
    options.alpha_channel_option = zaf::wic::BitmapAlphaChannelOption::IgnoreAlpha;
    auto image = zaf::wic::ImagingFactory::Instance().CreateBitmapFromHBITMAP(
        bitmap_handle,
        options);

    DeleteObject(bitmap_handle);
    return image;
}


static HBITMAP GetUserAvatarData(Id user_id) {

    if (user_id == InvalidId) {
        return nullptr;
    }

    const int avatar_count = 35;
    int avatar_index = (user_id % avatar_count) - 1;

    int resource_id = IDB_AVATAR1 + avatar_index;
    HANDLE handle = LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(resource_id), IMAGE_BITMAP, 0, 0, LR_SHARED);
    return reinterpret_cast<HBITMAP>(handle);
}