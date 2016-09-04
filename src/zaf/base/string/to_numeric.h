#pragma once

#include <cctype>
#include <cstdint>
#include <string>
#include <type_traits>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace zaf {
namespace internal {

template<typename CharType>
errno_t StringToInt64(const CharType* string, int radix, CharType** end, std::int64_t& value);

template<>
inline errno_t StringToInt64<char>(const char* string, int radix, char** end, std::int64_t& value) {
    errno = 0;
    value = std::strtoll(string, end, radix);
    return errno;
}

template<>
inline errno_t StringToInt64<wchar_t>(const wchar_t* string, int radix, wchar_t** end, std::int64_t& value) {
    errno = 0;
    value = std::wcstoll(string, end, radix);
    return errno;
}


template<typename CharType> 
errno_t StringToUint64(const CharType* string, int radix, CharType** end, std::uint64_t& value);

template<>
inline errno_t StringToUint64<char>(const char* string, int radix, char** end, std::uint64_t& value) {
    errno = 0;
    value = std::strtoull(string, end, radix);
    return errno;
}

template<>
inline errno_t StringToUint64<wchar_t>(const wchar_t* string, int radix, wchar_t** end, std::uint64_t& value) {
    errno = 0;
    value = std::wcstoull(string, end, radix);
    return errno;
}


template<typename NumericType, typename CharType>
struct StringToUnsignedNumericConverter {
    static bool Convert(const CharType* string, int radix, CharType** end, NumericType& value) {

        std::uint64_t temp_value = 0;
        auto error = StringToUint64(string, radix, end, temp_value);
        if (error) {
            return false;
        }

        if (temp_value > std::numeric_limits<NumericType>::max()) {
            return false;
        }

        value = static_cast<NumericType>(temp_value);
        return true;
    }
};


template<typename NumericType, typename CharType>
struct StringToSignedNumericConverter {
    static bool Convert(const CharType* string, int radix, CharType** end, NumericType& value) {

        if (radix == 10) {

            std::int64_t temp_value = 0;
            auto error = StringToInt64(string, radix, end, temp_value);
            if (error) {
                return false;
            }

            if (temp_value < std::numeric_limits<NumericType>::min() || temp_value > std::numeric_limits<NumericType>::max()) {
                return false;
            }

            value = static_cast<NumericType>(temp_value);
            return true;
        }
        else {

            typedef std::make_unsigned<NumericType>::type UnsignedNumericType;
            UnsignedNumericType temp_value = 0;
            bool is_succeeded = StringToUnsignedNumericConverter<UnsignedNumericType, CharType>::Convert(string, radix, end, temp_value);
            if (is_succeeded) {
                value = static_cast<NumericType>(temp_value);
                return true;
            }
            else {
                return false;
            }
        }
    }
};


template<typename NumericType, typename CharType>
bool ToNumeric(const CharType* string, int radix, CharType** end, NumericType& numeric_value) {

    typedef std::conditional<
        std::is_signed<NumericType>::value, 
        StringToSignedNumericConverter<NumericType, CharType>,
        StringToUnsignedNumericConverter<NumericType, CharType>
    >::type Converter;

    return Converter::Convert(string, radix, end, numeric_value);
}

}  // internal

/**
 Try to convert a C-style string to numeric value.
 */
template<typename NumericType, typename CharType>
bool TryToNumeric(const CharType* string, NumericType& numeric_value, int radix = 10) {

    if (*string == 0) {
        return false;
    }

    if (std::isspace(*string)) {
        return false;
    }

    //The sign is not allowed in non decimal number.
    if (radix != 10 && (*string == '-' || *string == '+')) {
        return false;
    }

    //The negative sign is not allowed in unsigned number.
    if (std::is_unsigned<NumericType>::value && (*string == '-')) {
        return false;
    }

    NumericType temp_value = 0;
    CharType* end = nullptr;
    bool is_succeeded = internal::ToNumeric(string, radix, &end, temp_value);
    if (! is_succeeded) {
        return false;
    }

    //Check whether the whole string has been converted.
    if (*end != 0) {
        return false;
    }

    numeric_value = temp_value;
    return true;
}


/**
 Try to convert a string to numeric value.

 @param string
    The string to be converted.

 @param numeric_value
    And output parameter to store the result.

 @param radix 
    The base of numeric, must be in the range 2-36. Defaults to 10.

 @return 
    Return true if succeeded; otherwise false.

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

 The string to be converted must match the numeric format exactly. 
 It meas that the string cannot contain any non-numeric character, 
 even blank characters.

 If radix is 10, the string is allowed to start with positive sign('+')
 or negative sign('-'). Otherwise, this two signs is not allowed. 

 Negative number cannnot be converted to unsigned types. For example,
 converting "-1" to unsigned int would failed. And, larger number cannot
 be converted to smaller types. For example, converting "1000" to unsigned
 char would failed.

 If the converting is failed, numeric_value would not be changed.
 */
template<typename NumericType, typename CharType>
bool TryToNumeric(const std::basic_string<CharType>& string, NumericType& numeric_value, int radix = 10) {
    return TryToNumeric(string.c_str(), numeric_value, radix);
}


/**
 Convert a C-style string to numeric value.
 */
template<typename NumericType, typename CharType>
NumericType ToNumeric(const CharType* string, int radix = 10) {

    NumericType numeric_value = 0;
    TryToNumeric(string, numeric_value, radix);
    return numeric_value;
}


/**
 Convert a string to numeric value.

 @param string
    The string to be converted.

 @param radix 
    The base of number. It must be in range 2-36.

 @return 
    Return 0 if the conversion failed.
 */
template<typename NumericType, typename CharType>
NumericType ToNumeric(const std::basic_string<CharType>& string, int radix = 10) {
    return ToNumeric<NumericType>(string.c_str(), radix);
}

}
