#pragma once

#include <string>

namespace zaf {
namespace internal {

template<typename C>
void Replace(
    std::basic_string<C>& string,
    std::basic_string_view<C> from,
    std::basic_string_view<C> to) {

    std::size_t index = 0;
    while (index < string.length()) {

        index = string.find(from, index);
        if (index == std::string::npos) {
            break;
        }

        string.replace(index, from.length(), to);
        index += to.length();
    }
}

}

inline void Replace(std::wstring& string, std::wstring_view from, std::wstring_view to) {
    internal::Replace<wchar_t>(string, from, to);
}

inline void Replace(std::string& string, std::string_view from, std::string_view to) {
    internal::Replace<char>(string, from, to);
}

}