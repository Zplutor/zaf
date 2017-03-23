#pragma once

#include <map>
#include <memory>
#include <zaf/graphic/image/image_source.h>
#include "entity/conversation.h"

class ConversationAvatarManager {
public:
    ConversationAvatarManager() { }
    ConversationAvatarManager(const ConversationAvatarManager&) = delete;
    ConversationAvatarManager& operator=(const ConversationAvatarManager&) = delete;

    zaf::ImageSource GetConversationAvatarImage(const std::shared_ptr<Conversation>& conversation);

private:
    zaf::ImageSource GenerateConversationAvatarImage(const std::shared_ptr<Conversation>& conversation);
    zaf::ImageSource CombineMultiUserConversationAvatarImage(const std::vector<Id>& member_ids);

private:
    std::map<Id, zaf::ImageSource> conversation_avatar_images_;
};