#pragma once

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <string>

namespace zaf {
namespace internal {

template<typename CharType, typename ChangeFunction>
void ChangeCase(std::basic_string<CharType>& string, ChangeFunction function) {
    std::transform(string.begin(), string.end(), string.begin(), function);
}

}


inline void Uppercase(std::string& string) {
    internal::ChangeCase(string, std::toupper);
}

inline void Uppercase(std::wstring& string) {
    internal::ChangeCase(string, std::towupper);
}


inline void Lowercase(std::string& string) {
    internal::ChangeCase(string, std::tolower);
}

inline void Lowercase(std::wstring& string) {
    internal::ChangeCase(string, std::towlower);
}


inline std::string ToUppercased(const std::string& string) {
    auto copied = string;
    Uppercase(copied);
    return copied;
}

inline std::wstring ToUppercased(const std::wstring& string) {
    auto copied = string;
    Uppercase(copied);
    return copied;
}


inline std::string ToLowercased(const std::string& string) {
    auto copied = string;
    Lowercase(copied);
    return copied;
}

inline std::wstring ToLowercased(const std::wstring& string) {
    auto copied = string;
    Lowercase(copied);
    return copied;
}


}