#pragma once

#include <string>
#include <zaf/base/string/encoding_conversion.h>

namespace zaf::internal {

template<typename T>
struct StringConversionShim {

};

template<>
struct StringConversionShim<std::string> {

    static const std::string& From(const std::string& string) {
        return string;
    }

    static std::string From(const std::wstring& string) {
        return ToUTF8String(string);
    }
};


template<>
struct StringConversionShim<std::wstring> {

    static std::wstring From(const std::string& string) {
        return FromUTF8String(string);
    }

    static const std::wstring& From(const std::wstring& string) {
        return string;
    }
};

}