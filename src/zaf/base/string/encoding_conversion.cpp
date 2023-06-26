#include <zaf/base/string/encoding_conversion.h>
#include <zaf/base/error/system_error.h>

namespace zaf {
namespace {

std::wstring InnerFromUTF8String(
    const char* utf8_string,
    std::size_t utf8_string_length,
    FromUTF8Flags flags) {

    if (utf8_string_length == 0) {
        return {};
    }

    int required_size = MultiByteToWideChar(
        CP_UTF8,
        static_cast<DWORD>(flags),
        utf8_string,
        static_cast<int>(utf8_string_length),
        nullptr,
        0);

    if (required_size == 0) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    std::wstring result;
    result.resize(required_size);

    int result_size = MultiByteToWideChar(
        CP_UTF8,
        static_cast<DWORD>(flags),
        utf8_string,
        static_cast<int>(utf8_string_length),
        &result[0],
        required_size);

    if (result_size == 0) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    result.resize(result_size);
    return result;
}

}

std::string ToUTF8String(std::wstring_view wide_string, ToUTF8Flags flags) {

    if (wide_string.empty()) {
        return {};
    }

    int required_size = WideCharToMultiByte(
        CP_UTF8,
        static_cast<DWORD>(flags),
        wide_string.data(),
        static_cast<int>(wide_string.size()),
        nullptr,
        0,
        nullptr,
        nullptr);

    if (required_size == 0) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    std::string result;
    result.resize(required_size);

    int result_size = WideCharToMultiByte(
        CP_UTF8,
        static_cast<DWORD>(flags),
        wide_string.data(),
        static_cast<int>(wide_string.size()),
        &result[0],
        required_size,
        nullptr,
        nullptr);

    if (result_size == 0) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    result.resize(result_size);
    return result;
}


std::wstring FromUTF8String(std::string_view utf8_string, FromUTF8Flags flags) {
    return InnerFromUTF8String(utf8_string.data(), utf8_string.length(), flags);
}


std::wstring FromUTF8String(std::u8string_view utf8_string, FromUTF8Flags flags) {
    return InnerFromUTF8String(
        reinterpret_cast<const char*>(utf8_string.data()),
        utf8_string.length(),
        flags);
}


}