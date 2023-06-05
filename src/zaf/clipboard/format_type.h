#pragma once

#include <Windows.h>

namespace zaf::clipboard {

enum class FormatType {
    Text = CF_UNICODETEXT,
    DropFiles = CF_HDROP,
};


inline constexpr FormatType MakePrivateFormatType(std::uint16_t private_id) {
    return static_cast<FormatType>(private_id + CF_PRIVATEFIRST);
}

}