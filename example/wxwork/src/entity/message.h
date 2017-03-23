#pragma once

#include <ctime>
#include <memory>
#include <string>
#include "entity/id.h"

class Message {
public:
    virtual ~Message() { }

public:
    Id id = InvalidId;
    Id conversation_id = InvalidId;
    Id sender_id = InvalidId;
    std::time_t sent_time = 0;
    std::wstring content;
};