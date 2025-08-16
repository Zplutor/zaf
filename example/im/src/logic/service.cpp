#include "logic/service.h"
#include "logic/data_generating/conversation_generating.h"
#include "logic/data_generating/message_generating.h"
#include "logic/data_generating/random.h"
#include "logic/data_generating/user_generating.h"

Service& Service::GetInstance() {

    static Service instance;
    return instance;
}


Service::Service() {

}


void Service::Initialize() {

    GenerateInitialData();
    StartMessageGeneratingTimer();
}


void Service::GenerateInitialData() {

    auto user_ids = GenerateInitialUsers();

    //Pick up the first users as current user.
    current_user_id_ = user_ids.front();
    user_ids.erase(user_ids.begin());

    GenerateInitialConversations(user_ids);
    GenerateInitialMessages();
}


std::vector<Id> Service::GenerateInitialUsers() {

    auto users = GenerateUsers();
    return user_storage_.AddUsers(users);
}


std::vector<Id> Service::GenerateInitialConversations(const std::vector<Id>& user_ids) {

    auto conversations = GenerateConversations(current_user_id_, user_ids, 100);
    return conversation_storage_.AddConversations(conversations);
}


std::vector<Id> Service::GenerateInitialMessages() {

    auto conversations = conversation_storage_.GetAllConversations();
    auto messages = GenerateMessages(conversations, 1000);
    return message_storage_.AddMessages(messages);
}


void Service::StartMessageGeneratingTimer() {

    if (message_generating_timer_ == nullptr) {
        message_generating_timer_ = std::make_shared<zaf::Timer>(zaf::Timer::Mode::OneShot);
        Disposables() += message_generating_timer_->TriggerEvent().Subscribe(
            std::bind(&Service::MessageGeneratingTimerTrigger, this));
    }

    auto interval_seconds = GenerateRandomInteger<std::size_t>(2, 10);
    message_generating_timer_->SetInterval(std::chrono::seconds(interval_seconds));
    message_generating_timer_->Start();
}


void Service::MessageGeneratingTimerTrigger() {

    auto all_conversations = conversation_storage_.GetAllConversations();
    if (! all_conversations.empty()) {

        auto conversation_index = GenerateRandomInteger<std::size_t>(0, all_conversations.size() - 1);
        auto conversation = all_conversations[conversation_index];
        GenerateMessageToConversation(conversation, false);
    }

    StartMessageGeneratingTimer();
}


void Service::GenerateMessageToConversation(
    const std::shared_ptr<Conversation>& conversation, 
    bool dont_user_current_user) {

    auto new_message = GenerateMessage(conversation);
    if (new_message->sender_id == current_user_id_ && dont_user_current_user) {
        return;
    }

    auto new_message_id = message_storage_.AddMessage(new_message);
    message_storage_.AddUnreadMessage(new_message_id);
    message_add_event_.AsObserver().OnNext(new_message);

    if (conversation->last_updated_time < new_message->sent_time) {
        conversation->last_updated_time = new_message->sent_time;
    }
    conversation_storage_.UpdateConversation(conversation);
    conversation_update_event_.AsObserver().OnNext(conversation);
}


void Service::SendMessageToConversation(const std::wstring& content, Id conversation_id) {

    auto new_message = std::make_shared<Message>();
    new_message->content = content;
    new_message->conversation_id = conversation_id;
    new_message->sent_time = std::time(nullptr);
    new_message->sender_id = current_user_id_;

    message_storage_.AddMessage(new_message);
    message_add_event_.AsObserver().OnNext(new_message);

    auto conversation = conversation_storage_.GetConversaton(conversation_id);
    if (conversation == nullptr) {
        return;
    }

    conversation->last_updated_time = new_message->sent_time;
    conversation_storage_.UpdateConversation(conversation);
    conversation_update_event_.AsObserver().OnNext(conversation);

    //Generate a reply message.
    auto random_integer = GenerateRandomInteger(0, 1);
    if (random_integer == 0) {
        return;
    }

    reply_timer_ = std::make_shared<zaf::Timer>(zaf::Timer::Mode::OneShot);
    reply_timer_->SetInterval(std::chrono::seconds(1));
    Disposables() += reply_timer_->TriggerEvent().Subscribe(std::bind([this, conversation]() {
        GenerateMessageToConversation(conversation, true);
    }));
    reply_timer_->Start();
}


void Service::RemoveConversationAllUnreadMessages(Id conversation_id) {

    bool is_succeeded = message_storage_.RemoveConversationAllUnreadMessage(conversation_id);
    if (! is_succeeded) {
        return;
    }

    auto conversation = conversation_storage_.GetConversaton(conversation_id);
    if (conversation != nullptr) {
        conversation_update_event_.AsObserver().OnNext(conversation);
    }
}