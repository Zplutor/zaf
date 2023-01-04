#include <zaf/base/string/encoding_conversion.h>
#include <Windows.h>

namespace zaf {

std::string ToUTF8String(const std::wstring& wide_string) {

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


std::wstring FromUTF8String(const std::string& utf8_string) {
    return FromUTF8String(utf8_string.data(), utf8_string.size());
}


std::wstring FromUTF8String(const char* utf8_string, std::size_t length) {

    int required_size = MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8_string,
        static_cast<int>(length),
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
        utf8_string,
        static_cast<int>(length),
        &result[0],
        required_size);

    result.resize(result_size);
    return result;
}


}