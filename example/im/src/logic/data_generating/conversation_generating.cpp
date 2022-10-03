#include "logic/data_generating/conversation_generating.h"
#include "logic/data_generating/random.h"
#include "logic/data_generating/string_generating.h"

static std::shared_ptr<Conversation> GenerateConversation(
    Id current_user_id, 
    const std::vector<Id>& other_user_ids);
static std::vector<Id> GenerateMembers(Id current_user_id, const std::vector<Id>& other_user_ids);
static std::time_t GenerateLastUpdatedTime();

std::vector<std::shared_ptr<Conversation>> GenerateConversations(
    Id current_user_id,
    const std::vector<Id>& other_user_ids,
    std::size_t count) {

    std::vector<std::shared_ptr<Conversation>> conversations;

    for (std::size_t index = 0; index < count; ++index) {
        conversations.push_back(GenerateConversation(current_user_id, other_user_ids));
    }

    return conversations;
}


static std::shared_ptr<Conversation> GenerateConversation(
    Id current_user_id,
    const std::vector<Id>& other_user_ids) {

    auto conversation = std::make_shared<Conversation>();
    conversation->members = GenerateMembers(current_user_id, other_user_ids);
    conversation->last_updated_time = GenerateLastUpdatedTime();

    if (conversation->members.size() > 2) {

        bool generate_title = GenerateRandomInteger(0, 1) == 1;
        if (generate_title) {
            conversation->title = GenerateNaturalString(10);
        }
    }

    return conversation;
}


static std::vector<Id> GenerateMembers(Id current_user_id, const std::vector<Id>& other_user_ids) {

    std::vector<Id> members{ current_user_id };

    auto other_member_ids = other_user_ids;

    auto other_member_count = GenerateRandomInteger<std::size_t>(1, other_member_ids.size());
    for (std::size_t count = 0; count < other_member_count; ++count) {

        std::size_t index = GenerateRandomInteger<std::size_t>(0, other_member_ids.size() - 1);
        members.push_back(other_member_ids[index]);

        other_member_ids.erase(std::next(other_member_ids.begin(), index));
    }

    return members;
}


static std::time_t GenerateLastUpdatedTime() {

    std::time_t current_time = std::time(nullptr);
    return GenerateRandomInteger<std::time_t>(
        current_time - 365 * 24 * 60 * 60, // a year ago
        current_time);
}