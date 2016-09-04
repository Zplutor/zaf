#pragma once

#include <string>

class InterpretContext {
public:
    InterpretContext(const std::wstring& command_text) :
        command_text_(command_text),
        current_index(0) {

    }

    std::size_t GetCurrentIndex() const {
        return current_index;
    }

    wchar_t GetCurrentChar() const {
        return command_text_[current_index];
    }

    bool Forward() {

        if (current_index < command_text_.length() - 1) {
            ++current_index;
            return true;
        }
        else {
            return false;
        }
    }

    bool Backward() {
        
        if (current_index > 0) {
            --current_index;
            return true;
        }
        else {
            return false;
        }
    }

    InterpretContext(InterpretContext&) = delete;
    InterpretContext& operator=(InterpretContext&) = delete;

private:
    std::wstring command_text_;
    std::size_t current_index;
};