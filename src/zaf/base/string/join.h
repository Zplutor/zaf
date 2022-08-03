#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

namespace zaf {
namespace internal {

template<typename Char, typename C, typename Appender>
std::basic_string<Char> JoinWithAppender(
    const C& container, 
    std::basic_string_view<Char> delimiter, 
    Appender appender) {

    std::basic_ostringstream<Char> stream;

    for (auto iterator = container.begin(); iterator != container.end(); ++iterator) {

        if (iterator != container.begin()) {
            stream << delimiter;
        }

        appender(stream, *iterator);
    }

    return stream.str();
}


template<typename Char, typename C, typename Converter>
std::basic_string<Char> JoinWithConverter(
    const C& container, 
    std::basic_string_view<Char> delimiter, 
    Converter converter) {

    return JoinWithAppender<Char>(container, delimiter, [converter](
        std::basic_ostringstream<Char>& stream,
        const typename C::value_type& element) {

        stream << converter(element);
    });
}


template<typename Char, typename C>
std::basic_string<Char> JoinWithoutConverter(
    const C& container, 
    std::basic_string_view<Char> delimiter) {

    return JoinWithAppender(container, delimiter, [](
        std::basic_ostringstream<Char>& stream,
        const typename C::value_type& element) {
    
        stream << element;
    });
}


constexpr const char* DefaultDelimiter = " ";
constexpr const wchar_t* DefaultWideDelimiter = L" ";

}


template<typename C>
std::string JoinAsString(const C& container) {
    return internal::JoinWithoutConverter<char>(
        container, 
        internal::DefaultDelimiter);
}

template<typename C>
std::wstring JoinAsWideString(const C& container) {
    return internal::JoinWithoutConverter<wchar_t>(
        container, 
        internal::DefaultWideDelimiter);
}


template<typename C>
std::string JoinAsString(const C& container, std::string_view delimiter) {
    return internal::JoinWithoutConverter<char>(container, delimiter);
}

template<typename C>
std::wstring JoinAsWideString(const C& container, std::wstring_view delimiter) {
    return internal::JoinWithoutConverter<wchar_t>(container, delimiter);
}


template<
    typename C, 
    typename Converter, 
    typename = typename std::enable_if<!std::is_convertible<Converter, std::string>::value>::type
>
std::string JoinAsString(const C& container, Converter&& converter) {
    return internal::JoinWithConverter<char>(
        container, 
        internal::DefaultDelimiter,
        std::forward<Converter>(converter));
}


template<
    typename C,
    typename Converter, 
    typename = typename std::enable_if<!std::is_convertible<Converter, std::wstring>::value>::type
>
std::wstring JoinAsWideString(const C& container, Converter&& converter) {
    return internal::JoinWithConverter<wchar_t>(
        container, 
        internal::DefaultWideDelimiter, 
        std::forward<Converter>(converter));
}


template<typename C, typename Converter>
std::string JoinAsString(const C& container, std::string_view delimiter, Converter&& converter) {
    return internal::JoinWithConverter<char>(
        container, 
        delimiter, 
        std::forward<Converter>(converter));
}

template<typename C, typename Converter>
std::wstring JoinAsWideString(
    const C& container, 
    std::wstring_view delimiter, 
    Converter&& converter) {

    return internal::JoinWithConverter<wchar_t>(
        container,
        delimiter,
        std::forward<Converter>(converter));
}

}