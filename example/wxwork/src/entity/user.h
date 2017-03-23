#pragma once

#include <string>
#include "entity/id.h"

class User {
public:
    enum class Gender {
        Unknown,
        Female,
        Male,
    };

public:
    Id id = InvalidId;
    std::wstring name;
    Gender gender = Gender::Unknown;
};