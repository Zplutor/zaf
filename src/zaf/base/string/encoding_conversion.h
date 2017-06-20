#pragma once

#include <string>

namespace zaf {

std::string ToUtf8String(const std::wstring& wide_string);

std::wstring FromUtf8String(const std::string& utf8_string);
std::wstring FromUtf8String(const char* utf8_string, std::size_t length);

}