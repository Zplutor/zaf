#pragma once

#include <string_view>

namespace zaf {

std::string ToUTF8String(std::wstring_view wide_string);

std::wstring FromUTF8String(std::string_view utf8_string);

}