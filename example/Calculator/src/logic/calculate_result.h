#pragma once

#include <cstdint>

class CalculateResult {
public:
    enum class Error {
        None,
        SyntaxError,
        SemanticsError,
    };

public:
    Error error = Error::None;
    std::wstring text;
};