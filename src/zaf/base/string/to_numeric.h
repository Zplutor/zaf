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

/**
 Defines options that control the format when converting a string to numeric value.
 */
class ToNumericOptions {
public:
    /**
     Set base.

     For integer numeric, valid base is in range 2 - 36.
     For floating point numeric, valid base is 10.

     The default base is 10.
     */
    ToNumericOptions& Base(int base) {
        base_ = base;
        return *this;
    }

    /**
     Get base.
     */
    int Base() const {
        return base_;
    }

private:
    int base_ = 10;
};


/**
 Try to convert a string to numeric value.

 @param string
     The string to be converted.

 @param numeric_value
     An output parameter to store the result.

 @param options
     Options used in conversion.

 @return
     Return true if succeeded; otherwise false.

 Available types of numeric value are listed below:
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
     float
     double
     long double

 The string to be converted must match the numeric format exactly.
 It meas that the string cannot contain any non-numeric character,
 even blank characters.

 If base is 10, the string is allowed to start with positive sign('+')
 or negative sign('-'). Otherwise, this two signs is not allowed.

 Negative number cannnot be converted to unsigned types. For example,
 converting "-1" to unsigned int would failed. And, larger number cannot
 be converted to smaller types. For example, converting "1000" to unsigned
 char would failed.

 If the conversion is failed, numeric_value would not be changed.
 */
template<typename NumericType, typename CharType>
bool TryToNumeric(const std::basic_string<CharType>& string, NumericType& numeric_value, const ToNumericOptions& options = ToNumericOptions());

/**
 Try to convert a C-style string to numeric value.
 */
template<typename NumericType, typename CharType>
bool TryToNumeric(const CharType* string, NumericType& numeric_value, const ToNumericOptions& options = ToNumericOptions());


/**
 Convert a string to numeric value.

 @param string
     The string to be converted.

 @param options
     Options used in conversion.

 @return
     Return 0 if the conversion failed.
*/
template<typename NumericType, typename CharType>
NumericType ToNumeric(const std::basic_string<CharType>& string, const ToNumericOptions& options = ToNumericOptions());

/**
 Convert a C-style string to numeric value.
 */
template<typename NumericType, typename CharType>
NumericType ToNumeric(const CharType* string, const ToNumericOptions& options = ToNumericOptions());


namespace internal {

template<typename CharType>
errno_t StringToInt64(const CharType* string, int base, CharType** end, std::int64_t& value);

template<>
inline errno_t StringToInt64<char>(const char* string, int base, char** end, std::int64_t& value) {
    errno = 0;
    value = std::strtoll(string, end, base);
    return errno;
}

template<>
inline errno_t StringToInt64<wchar_t>(const wchar_t* string, int base, wchar_t** end, std::int64_t& value) {
    errno = 0;
    value = std::wcstoll(string, end, base);
    return errno;
}


template<typename CharType> 
errno_t StringToUint64(const CharType* string, int base, CharType** end, std::uint64_t& value);

template<>
inline errno_t StringToUint64<char>(const char* string, int base, char** end, std::uint64_t& value) {
    errno = 0;
    value = std::strtoull(string, end, base);
    return errno;
}

template<>
inline errno_t StringToUint64<wchar_t>(const wchar_t* string, int base, wchar_t** end, std::uint64_t& value) {
    errno = 0;
    value = std::wcstoull(string, end, base);
    return errno;
}


template<typename NumericType, typename CharType>
errno_t StringToFloat(const CharType* string, CharType** end, NumericType& value);

template<>
inline errno_t StringToFloat<float, char>(const char* string, char** end, float& value) {
    errno = 0;
    value = std::strtof(string, end);
    return errno;
}

template<>
inline errno_t StringToFloat<float, wchar_t>(const wchar_t* string, wchar_t** end, float& value) {
    errno = 0;
    value = std::wcstof(string, end);
    return errno;
}

template<>
inline errno_t StringToFloat<double, char>(const char* string, char** end, double& value) {
    errno = 0;
    value = std::strtod(string, end);
    return errno;
}

template<>
inline errno_t StringToFloat<double, wchar_t>(const wchar_t* string, wchar_t** end, double& value) {
    errno = 0;
    value = std::wcstod(string, end);
    return errno;
}

template<>
inline errno_t StringToFloat<long double, char>(const char* string, char** end, long double& value) {
    errno = 0;
    value = std::strtold(string, end);
    return errno;
}

template<>
inline errno_t StringToFloat<long double, wchar_t>(const wchar_t* string, wchar_t** end, long double& value) {
    errno = 0;
    value = std::wcstold(string, end);
    return errno;
}


template<typename NumericType, typename CharType>
struct StringToUnsignedIntegerConverter {
    static bool Convert(const CharType* string, const ToNumericOptions& options, CharType** end, NumericType& value) {

        std::uint64_t temp_value = 0;
        auto error = StringToUint64(string, options.Base(), end, temp_value);
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
struct StringToSignedIntegerConverter {
    static bool Convert(const CharType* string, const ToNumericOptions& options, CharType** end, NumericType& value) {

        if (options.Base() == 10) {

            std::int64_t temp_value = 0;
            auto error = StringToInt64(string, options.Base(), end, temp_value);
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
            bool is_succeeded = StringToUnsignedIntegerConverter<UnsignedNumericType, CharType>::Convert(string, options, end, temp_value);
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
struct StringToFloatConverter {
    static bool Convert(const CharType* string, const ToNumericOptions& options, CharType** end, NumericType& value) {

        if (options.Base() != 10) {
            return false;
        }

        NumericType temp_value();
        auto error = StringToLongDouble(string, end, temp_value);
        if (error) {
            return false;
        }

        value = static_cast<NumericType>(temp_value);
        return true;
    }
};


template<typename NumericType, typename CharType>
bool ToNumeric(const CharType* string, const ToNumericOptions& options, CharType** end, NumericType& numeric_value) {

    typedef std::conditional <
        std::is_floating_point<NumericType>::value,
        StringToFloatConverter<NumericType, CharType>,
        std::conditional<
            std::is_signed<NumericType>::value,
            StringToSignedIntegerConverter<NumericType, CharType>,
            StringToUnsignedIntegerConverter<NumericType, CharType>
        >::type
    >::type Converter;

    return Converter::Convert(string, options, end, numeric_value);
}

}  // internal


template<typename NumericType, typename CharType>
bool TryToNumeric(const std::basic_string<CharType>& string, NumericType& numeric_value, const ToNumericOptions& options) {
    return TryToNumeric(string.c_str(), numeric_value, options);
}

template<typename NumericType, typename CharType>
bool TryToNumeric(const CharType* string, NumericType& numeric_value, const ToNumericOptions& options) {

    if (*string == 0) {
        return false;
    }

    if (std::isspace(*string)) {
        return false;
    }

    //The sign is not allowed in non decimal number.
    if (options.Base() != 10 && (*string == '-' || *string == '+')) {
        return false;
    }

    //The negative sign is not allowed in unsigned number.
    if (std::is_unsigned<NumericType>::value && (*string == '-')) {
        return false;
    }

    NumericType temp_value = 0;
    CharType* end = nullptr;
    bool is_succeeded = internal::ToNumeric(string, options, &end, temp_value);
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


template<typename NumericType, typename CharType>
NumericType ToNumeric(const std::basic_string<CharType>& string, const ToNumericOptions& options) {
    return ToNumeric<NumericType, CharType>(string.c_str(), options);
}

template<typename NumericType, typename CharType>
NumericType ToNumeric(const CharType* string, const ToNumericOptions& options) {

    NumericType numeric_value = 0;
    TryToNumeric<NumericType, CharType>(string, numeric_value, options);
    return numeric_value;
}

}
