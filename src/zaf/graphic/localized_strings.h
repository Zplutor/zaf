#pragma once

#include <dwrite.h>
#include <cstdint>
#include <string>
#include <zaf/base/error.h>

namespace zaf {

/**
 Represents a collection of strings indexed by locale name.   
 */
class LocalizedStrings {
public:
    explicit LocalizedStrings(IDWriteLocalizedStrings* handle) : handle_(handle) { }

    ~LocalizedStrings() {
        handle_->Release();
    }

    /**
     Gets the number of language/string pairs.
     */
    std::uint32_t GetCount() const {
        return handle_->GetCount();
    }

    /**
     Get the locale name at specified index.

     @param index
         Zero-based index of the locale name.

     @return 
         Return an empty string if failed.
     */
    const std::wstring GetLocaleName(std::uint32_t index, std::error_code& error_code) const;

    const std::wstring GetLocaleName(std::uint32_t index) const {
        std::error_code error_code;
        auto result = GetLocaleName(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Get the string at specified index.

     @param index
         Zero-based index of the string.

     @return 
         Return an empty string if failed.
     */
    const std::wstring GetString(std::uint32_t index, std::error_code& error_code) const;

    const std::wstring GetString(std::uint32_t index) const {
        std::error_code error_code;
        auto result = GetString(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    IDWriteLocalizedStrings* GetHandle() const {
        return handle_;
    }

    LocalizedStrings(const LocalizedStrings&) = delete;
    LocalizedStrings& operator=(const LocalizedStrings&) = delete;

private:
    IDWriteLocalizedStrings* handle_;
};

}