#pragma once

#include <memory>
#include <vector>
#include "entity/conversation.h"

std::vector<std::shared_ptr<Conversation>> GenerateConversations(
    Id current_user_id, 
    const std::vector<Id>& other_user_ids,
    std::size_t count);