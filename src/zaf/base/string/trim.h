#pragma once

#include <cctype>
#include <cwctype>
#include <string>
#include <string_view>
#include <type_traits>

namespace zaf {
namespace internal {

inline bool IsSpace(char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
}

inline bool IsSpace(wchar_t ch) {
    return std::iswspace(ch);
}

}


template<typename C, typename P>
void TrimStart(std::basic_string<C>& string, P&& predicate) {

    std::size_t last_trim_index{};
    for (last_trim_index = 0; last_trim_index < string.length(); ++last_trim_index) {

        if (!predicate(string[last_trim_index])) {
            break;
        }
    }

    if (last_trim_index != 0) {
        string.erase(0, last_trim_index);
    }
}


template<typename C>
void TrimStart(std::basic_string<C>& string) {
    TrimStart(string, [](C ch) {
        return internal::IsSpace(ch);
    });
}


template<typename C, typename P>
void TrimEnd(std::basic_string<C>& string, P&& predicate) {

    auto iterator = string.rbegin();
    for (; iterator != string.rend(); ++iterator) {

        if (!predicate(*iterator)) {
            break;
        }
    }

    std::size_t remain_length = std::distance(iterator, string.rend());
    string.resize(remain_length);
}


template<typename C>
void TrimEnd(std::basic_string<C>& string) {
    TrimEnd(string, [](C ch) {
        return internal::IsSpace(ch);
    });
}


template<typename C, typename P>
void Trim(std::basic_string<C>& string, P&& predicate) {
    TrimEnd(string, predicate);
    TrimStart(string, predicate);
}


template<typename C>
void Trim(std::basic_string<C>& string) {

    Trim(string, [](C ch) {
        return internal::IsSpace(ch);
    });
}


template<typename P>
std::string ToTrimmed(std::string_view string, P&& predicate) {

    std::string result{ string.data(), string.length() };
    Trim(result, std::forward<P>(predicate));
    return result;
}


template<typename P>
std::wstring ToTrimmed(std::wstring_view string, P&& predicate) {

    std::wstring result{ string.data(), string.length() };
    Trim(result, std::forward<P>(predicate));
    return result;
}


inline std::string ToTrimmed(std::string_view string) {

    std::string result{ string.data(), string.length() };
    Trim(result);
    return result;
}


inline std::wstring ToTrimmed(std::wstring_view string) {

    std::wstring result{ string.data(), string.length() };
    Trim(result);
    return result;
}

}