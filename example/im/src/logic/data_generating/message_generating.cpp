#include "logic/data_generating/message_generating.h"
#include "logic/data_generating/random.h"
#include "logic/data_generating/string_generating.h"

std::shared_ptr<Message> GenerateMessage(const std::shared_ptr<Conversation>& conversation) {

    auto message = std::make_shared<Message>();
    message->conversation_id = conversation->id;
    message->sent_time = std::time(nullptr);
    message->content = GenerateNaturalString(GenerateRandomInteger<std::size_t>(1, 50));

    auto sender_index = GenerateRandomInteger<std::size_t>(0, conversation->members.size() - 1);
    message->sender_id = conversation->members[sender_index];
    return message;
}


std::vector<std::shared_ptr<Message>> GenerateMessages(
    const std::vector<std::shared_ptr<Conversation>>& conversations,
    std::size_t count) {

    std::vector<std::shared_ptr<Message>> messages;

    for (std::size_t current_count = 0; current_count < count; ++current_count) {

        std::size_t conversation_index = GenerateRandomInteger<std::size_t>(0, conversations.size() - 1);
        auto message = GenerateMessage(conversations[conversation_index]);
        messages.push_back(message);
    }

    return messages;
}