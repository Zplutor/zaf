#pragma once

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>

namespace zaf {

/**
 Defines options that control the format when converting a numeric value 
 to string.
 */
class ToStringOptions {
public:
    /**
     Set base.

     For integer numeric, valid base is in range 2 - 36.
     For floating point numeric, valid base is 10.

     The default base is 10.
     */
    ToStringOptions& Base(int base) {
        base_ = base;
        return *this;
    }

    /**
     Get base.
     */
    int Base() const {
        return base_;
    }

    /**
     Set precision.

     This option specifies the count of digits reserved after the decimal point.
     It is for floating point numeric only.

     The default precision is 6.
     */
    ToStringOptions& Precision(int precision) {
        precision_ = precision;
        return *this;
    }

    /**
     Get precision.
     */
    int Precision() const {
        return precision_;
    }

    /**
     Set whether use scientific notation.

     This option is for floating point numeric only.

     The default value is false.
     */
    ToStringOptions& UseScientificNotation(bool use) {
        use_scientific_notation_ = use;
        return *this;
    }

    /**
     Get whether use scientific notation.
     */
    bool UseScientificNotation() const {
        return use_scientific_notation_;
    }

private:
    int base_ = 10;
    int precision_ = 6;
    bool use_scientific_notation_ = false;
};


/**
 Convert a numeric value to string.

 @param numeric_value
     The numeric value to be converted.

 @param options
     Options applied to conversion.

 @return
     Return an empty string if failed.

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

 If NumericType is char or wchar_t, this function would return a number string,
 other than a character string. For example, ToString('0') would get string "48".
 */
template<typename NumericType>
const std::string ToString(NumericType numeric_type, const ToStringOptions& options = ToStringOptions());


/**
 Convert a numeric value to wide string.

 See also ToString.
 */
template<typename NumericType>
const std::wstring ToWideString(NumericType numeric_type, const ToStringOptions& options = ToStringOptions());


namespace internal {

template<typename CharType>
errno_t Int64ToString(std::int64_t value, int base, std::size_t buffer_size, CharType* buffer);

template<>
inline errno_t Int64ToString<char>(std::int64_t value, int base, std::size_t buffer_size, char* buffer) {
    return _i64toa_s(value, buffer, buffer_size, base);
}

template<>
inline errno_t Int64ToString<wchar_t>(std::int64_t value, int base, std::size_t buffer_size, wchar_t* buffer) {
    return _i64tow_s(value, buffer, buffer_size, base);
}


template<typename CharType>
errno_t Uint64ToString(std::uint64_t value, int base, std::size_t buffer_size, CharType* buffer);

template<>
inline errno_t Uint64ToString<char>(std::uint64_t value, int base, std::size_t buffer_size, char* buffer) {
    return _ui64toa_s(value, buffer, buffer_size, base);
}

template<>
inline errno_t Uint64ToString<wchar_t>(std::uint64_t value, int base, std::size_t buffer_size, wchar_t* buffer) {
    return _ui64tow_s(value, buffer, buffer_size, base);
}


template<typename CharType, typename NumericType>
struct SignedIntegerToStringConverter {
    static errno_t Convert(NumericType numeric_value, const ToStringOptions& options, std::size_t buffer_size, CharType* buffer) {
        if (options.Base() == 10) {
            return Int64ToString(numeric_value, options.Base(), buffer_size, buffer);
        }
        else {
            return Uint64ToString(static_cast<std::make_unsigned<NumericType>::type>(numeric_value), options.Base(), buffer_size, buffer);
        }
    }
};

template<typename CharType, typename NumericType>
struct UnsignedIntegerToStringConverter {
    static errno_t Convert(NumericType numeric_value, const ToStringOptions& options, std::size_t buffer_size, CharType* buffer) {
        return Uint64ToString(numeric_value, options.Base(), buffer_size, buffer);
    }
};

template<typename CharType, typename NumericType>
struct IntegerToStringConverter {
    static std::basic_string<CharType> Convert(NumericType numeric_value, const ToStringOptions& options) {

        typedef std::conditional<
            std::is_signed<NumericType>::value,
            SignedIntegerToStringConverter<CharType, NumericType>,
            UnsignedIntegerToStringConverter<CharType, NumericType>
        >::type Converter;

        const std::size_t buffer_size = 65;
        CharType buffer[buffer_size] = { 0 };
        auto error = Converter::Convert(numeric_value, options, buffer_size, buffer);

        if (!error) {
            return buffer;
        }
        else {
            return std::basic_string<CharType>();
        }
    }
};


template<typename CharType, typename NumericType>
struct FloatToStringConverter {
    static std::basic_string<CharType> Convert(NumericType numeric_type, const ToStringOptions& options) {

        if (options.Base() != 10) {
            return std::basic_string<CharType>();
        }

        std::basic_ostringstream<CharType> stream;
        stream.imbue(std::locale::classic());
        if (options.UseScientificNotation()) {
            stream << std::scientific;
        }
        else {
            stream << std::fixed;
        }
        stream << std::setprecision(options.Precision());
        stream << numeric_type;
        return stream.str();
    }
};


template<typename CharType, typename NumericType>
const std::basic_string<CharType> ToString(NumericType numeric_value, const ToStringOptions& options) {

    typedef std::conditional<
        std::is_floating_point<NumericType>::value,
        FloatToStringConverter<CharType, NumericType>,
        IntegerToStringConverter<CharType, NumericType>
    >::type Converter;

    return Converter::Convert(numeric_value, options);
}

} // internal


template<typename NumericType>
const std::string ToString(NumericType numeric_value, const ToStringOptions& options) {
    return internal::ToString<char>(numeric_value, options);
}


template<typename NumericType>
const std::wstring ToWideString(NumericType numeric_value, const ToStringOptions& options) {
    return internal::ToString<wchar_t>(numeric_value, options);
}

}