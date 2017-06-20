#include <zaf/base/string/encoding_conversion.h>
#include <codecvt>

namespace zaf {

std::string ToUtf8String(const std::wstring& wide_string) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wide_string);
}


std::wstring FromUtf8String(const std::string& utf8_string) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8_string);
}


std::wstring FromUtf8String(const char* utf8_string, std::size_t length) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8_string, utf8_string + length);
}


}