#pragma once

#include <memory>
#include <string>

class CalculateResult;

class Calculator {
public:
    std::shared_ptr<CalculateResult> Calculate(const std::wstring& command_text);
};