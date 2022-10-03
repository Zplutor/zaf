#pragma once

#include <memory>
#include "entity/conversation.h"

std::wstring GenerateConversationTitle(const std::shared_ptr<Conversation>& conversation);