#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>

namespace zaf::clipboard {

enum class FormatType {
    Indeterminate = 0,
    Text = CF_UNICODETEXT,
    DropFiles = CF_HDROP,
};


inline constexpr FormatType MakePrivateFormatType(std::uint16_t private_id) {
    return static_cast<FormatType>(private_id + CF_PRIVATEFIRST);
}

FormatType MakeSharedFormatType(const std::wstring& format_name);

}