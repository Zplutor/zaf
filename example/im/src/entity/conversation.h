#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <vector>
#include "entity/id.h"
#include "entity/user.h"

class Conversation {
public:
    Id id = InvalidId;
    std::wstring title;
    std::time_t last_updated_time = 0;
    std::vector<Id> members;
    bool is_sticked = false;
    bool is_silent = false;
};