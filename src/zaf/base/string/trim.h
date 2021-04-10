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
void Trim(std::basic_string<C>& string, P&& predicate) {

    if (string.empty()) {
        return;
    }

    std::size_t end_index{};
    for (end_index = string.length() - 1; end_index > 0; --end_index) {

        if (!predicate(string[end_index])) {
            break;
        }
    }

    end_index++;

    if (end_index != string.length()) {
        string.erase(end_index, string.length() - end_index);
    }

    std::size_t begin_index{};
    for (begin_index = 0; begin_index < end_index; ++begin_index) {

        if (!predicate(string[begin_index])) {
            break;
        }
    }

    if (begin_index != 0) {
        string.erase(0, begin_index);
    }
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