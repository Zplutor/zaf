#pragma once

#include <system_error>
#include <zaf/base/error/error.h>

namespace zaf {

enum class BasicErrc {

    NameNotFound = 1,
    InvalidValue,
    InvalidCast,
    InvalidOperation,
};

const std::error_category& BasicCategory();

inline std::error_code make_error_code(zaf::BasicErrc errc) noexcept {
    return std::error_code{ static_cast<int>(errc), zaf::BasicCategory() };
}

}


namespace std {

template<>
struct is_error_code_enum<zaf::BasicErrc> : public true_type { };

}