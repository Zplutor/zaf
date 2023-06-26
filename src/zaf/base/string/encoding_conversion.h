#pragma once

#include <Windows.h>
#include <string_view>

namespace zaf {

/**
Flags to control conversion between UTF-8 encoded string and wide string.
*/
enum class UTF8ConversionFlags {
    None = 0,

    /**
    Conversion will fail if there are invalid chars in the string.
    */
    FailOnInvalidChars = MB_ERR_INVALID_CHARS,
};


/**
Converts a wide string to an UTF-8 encoded string.

@param wide_string
    The wide string being converted.

@param flags
    Flags to control the conversion.

@return
    An UTF-8 encoded string after conversion.

@throw zaf::Error
    Conversion fails.
*/
std::string ToUTF8String(
    std::wstring_view wide_string, 
    UTF8ConversionFlags flags = UTF8ConversionFlags::None);


/**
Converts an UTF-8 encoded string to a wide string.

@param utf8_string 
    The UTF-8 encoded string being converted.

@param flags
    Flags to control the conversion.

@return
    A wide string after conversion.

@throw zaf::Error
    Conversion fails.
*/
std::wstring FromUTF8String(
    std::string_view utf8_string,
    UTF8ConversionFlags flags = UTF8ConversionFlags::None);

std::wstring FromUTF8String(
    std::u8string_view utf8_string, 
    UTF8ConversionFlags flags = UTF8ConversionFlags::None);

}