#pragma once

#include <cstdint>

class Value {
public:
    enum class Type {
        Integer,
    };

public:
    Type type = Type::Integer;
    std::int64_t integer_value = 0;
};