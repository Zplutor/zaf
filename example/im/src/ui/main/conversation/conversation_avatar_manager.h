#pragma once

#include <map>
#include <memory>
#include <zaf/graphic/wic/bitmap_source.h>
#include "entity/conversation.h"

class ConversationAvatarManager {
public:
    ConversationAvatarManager() { }
    ConversationAvatarManager(const ConversationAvatarManager&) = delete;
    ConversationAvatarManager& operator=(const ConversationAvatarManager&) = delete;

    zaf::wic::BitmapSource GetConversationAvatarImage(const std::shared_ptr<Conversation>& conversation);

private:
    zaf::wic::BitmapSource GenerateConversationAvatarImage(const std::shared_ptr<Conversation>& conversation);
    zaf::wic::BitmapSource CombineMultiUserConversationAvatarImage(const std::vector<Id>& member_ids);

private:
    std::map<Id, zaf::wic::BitmapSource> conversation_avatar_images_;
};