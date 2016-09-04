#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace zaf {
namespace internal {

template<typename CharType>
errno_t Int64ToString(std::int64_t value, int radix, std::size_t buffer_size, CharType* buffer);

template<>
inline errno_t Int64ToString<char>(std::int64_t value, int radix, std::size_t buffer_size, char* buffer) {
    return _i64toa_s(value, buffer, buffer_size, radix);
}

template<>
inline errno_t Int64ToString<wchar_t>(std::int64_t value, int radix, std::size_t buffer_size, wchar_t* buffer) {
    return _i64tow_s(value, buffer, buffer_size, radix);
}


template<typename CharType>
errno_t Uint64ToString(std::uint64_t value, int radix, std::size_t buffer_size, CharType* buffer);

template<>
inline errno_t Uint64ToString<char>(std::uint64_t value, int radix, std::size_t buffer_size, char* buffer) {
    return _ui64toa_s(value, buffer, buffer_size, radix);
}

template<>
inline errno_t Uint64ToString<wchar_t>(std::uint64_t value, int radix, std::size_t buffer_size, wchar_t* buffer) {
    return _ui64tow_s(value, buffer, buffer_size, radix);
}


template<typename CharType, typename NumericType>
struct SignedNumericToStringConverter {
    static errno_t Convert(NumericType numeric_value, int radix, std::size_t buffer_size, CharType* buffer) {
        if (radix == 10) {
            return Int64ToString(numeric_value, radix, buffer_size, buffer);
        }
        else {
            return Uint64ToString(static_cast<std::make_unsigned<NumericType>::type>(numeric_value), radix, buffer_size, buffer);
        }
    }
};

template<typename CharType, typename NumericType>
struct UnsignedNumericToStringConverter {
    static errno_t Convert(NumericType numeric_value, int radix, std::size_t buffer_size, CharType* buffer) {
        return Uint64ToString(numeric_value, radix, buffer_size, buffer);
    }
};


template<typename CharType, typename NumericType>
const std::basic_string<CharType> ToString(NumericType numeric_value, int radix) {

    typedef std::conditional<
        std::is_signed<NumericType>::value,
        SignedNumericToStringConverter<CharType, NumericType>,
        UnsignedNumericToStringConverter<CharType, NumericType>
    >::type Converter;

    const std::size_t buffer_size = 65;
    CharType buffer[buffer_size] = { 0 };
    auto error = Converter::Convert(numeric_value, radix, buffer_size, buffer);

    if (! error) {
        return buffer;
    }
    else {
        return std::basic_string<CharType>();
    }
}

} // internal

/**
 Convert a numeric value to string.

 @param numeric_value 
    The numeric value to be converted.

 @param radix 
    The base of value, must be in the range 2-36.

 @return 
    Return an empty string if failed.

 The available types of numeric value are listed below:
 char
 signed char
 unsigned char
 wchar_t
 short
 unsigned short
 int
 unsigned int
 long
 unsigned long
 long long
 unsigned long long

 If NumericType is char or wchar_t, this function would return a number string, 
 other than a character string. For example, ToString('0') would get string "48".
 */
template<typename NumericType>
const std::string ToString(NumericType numeric_value, int radix = 10) {
    return internal::ToString<char>(numeric_value, radix);
}


/**
 Convert a numeric value to wide string.

 See also ToString.
 */
template<typename NumericType>
const std::wstring ToWideString(NumericType numeric_value, int radix = 10) {
    return internal::ToString<wchar_t>(numeric_value, radix);
}

}