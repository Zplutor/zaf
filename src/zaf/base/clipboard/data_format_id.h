#pragma once

#include <Windows.h>

namespace zaf {

class DataFormatID {
public:
    static constexpr std::uint16_t Text = CF_UNICODETEXT;

public:
    DataFormatID() = default;
    explicit DataFormatID(std::uint16_t value) : value_(value) {

    }

    explicit constexpr operator std::uint16_t() const {
        return value_;
    }

private:
    std::uint16_t value_{};
};

}