#pragma once

#include <zaf/base/timer.h>
#include "storage/conversation_storage.h"
#include "storage/message_storage.h"
#include "storage/user_storage.h"

class Service {
public:
    typedef zaf::Event<const std::shared_ptr<Conversation>> ConversationUpdateEvent;

public:
    static Service& GetInstance();

public:
    void Initialize();

    Id GetCurrentUserId() {
        return current_user_id_;
    }

    std::vector<std::shared_ptr<User>> GetUsers(const std::vector<Id>& user_ids) {
        return user_storage_.GetUsers(user_ids);
    }

    std::shared_ptr<User> GetUser(Id user_id) {
        auto users = GetUsers({ user_id });
        return users.empty() ? nullptr : users.front();
    }

    std::vector<std::shared_ptr<Conversation>> GetAllConversations() {
        return conversation_storage_.GetAllConversations();
    }

    std::shared_ptr<Message> GetConversationLastMessage(Id conversation_id) {
        return message_storage_.GetConversationLastMessage(conversation_id);
    }

    std::size_t GetConversationUnreadMessageCount(Id conversation_id) {
        return message_storage_.GetConversationUnreadMessageCount(conversation_id);
    }

    ConversationUpdateEvent::Proxy GetConversationUpdateEvent() {
        return ConversationUpdateEvent::Proxy(conversation_update_event_);
    }

private:
    Service();

    void GenerateInitialData();
    std::vector<Id> GenerateInitialUsers();
    std::vector<Id> GenerateInitialConversations(const std::vector<Id>& user_ids);
    std::vector<Id> GenerateInitialMessages();

    void StartMessageGeneratingTimer();
    void MessageGeneratingTimerTrigger();

private:
    Id current_user_id_ = InvalidId;

    ConversationStorage conversation_storage_;
    MessageStorage message_storage_;
    UserStorage user_storage_;

    std::shared_ptr<zaf::Timer> message_generating_timer_;

    ConversationUpdateEvent conversation_update_event_;
};