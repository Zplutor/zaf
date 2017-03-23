#pragma once

#include <vector>
#include "entity/conversation.h"
#include "entity/message.h"

std::shared_ptr<Message> GenerateMessage(const std::shared_ptr<Conversation>& conversation);

std::vector<std::shared_ptr<Message>> GenerateMessages(
    const std::vector<std::shared_ptr<Conversation>>& conversations, 
    std::size_t count);