#include <zaf/base/string/encoding_conversion.h>
#include <Windows.h>

namespace zaf {

std::string ToUTF8String(std::wstring_view wide_string) {

    int required_size = WideCharToMultiByte(
        CP_UTF8,
        0,
        wide_string.data(),
        static_cast<int>(wide_string.size()),
        nullptr,
        0,
        nullptr,
        nullptr);

    if (required_size <= 0) {
        return {};
    }

    std::string result;
    result.resize(required_size);

    int result_size = WideCharToMultiByte(CP_UTF8,
        0,
        wide_string.data(),
        static_cast<int>(wide_string.size()),
        &result[0],
        required_size,
        nullptr,
        nullptr);

    result.resize(result_size);
    return result;
}


std::wstring FromUTF8String(std::string_view utf8_string) {

    int required_size = MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8_string.data(),
        static_cast<int>(utf8_string.size()),
        nullptr,
        0);

    if (required_size <= 0) {
        return {};
    }

    std::wstring result;
    result.resize(required_size);

    int result_size = MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8_string.data(),
        static_cast<int>(utf8_string.size()),
        &result[0],
        required_size);

    result.resize(result_size);
    return result;
}


}