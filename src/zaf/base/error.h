#pragma once

#include <WinError.h>
#include <system_error>
#include <zaf/base/assert.h>

namespace zaf {

inline const std::error_category& ComCategory() {
    
    class ComErrorCategory : public std::error_category {
    public:
        const char* name() const override {
            return "COM";
        }

        std::string message(int condition) const override {
            return std::string();
        }
    };

    static ComErrorCategory category;
    return category;
}


inline const std::error_code MakeComErrorCode(HRESULT com_error_code) {
    return std::error_code(static_cast<int>(com_error_code), ComCategory());
}


inline const std::error_code MakeSystemErrorCode(DWORD system_error_code) {
    return std::error_code(static_cast<int>(system_error_code), std::system_category());
}


inline bool IsComErrorCode(const std::error_code& error_code, HRESULT com_error_code) {
    return (error_code.category() == ComCategory()) && (error_code.value() == com_error_code);
}


inline bool IsSystemErrorCode(const std::error_code& error_code, DWORD system_error_code) {
    return (error_code.category() == ComCategory()) && (error_code.value() == system_error_code);
}


inline bool IsSucceeded(const std::error_code& error_code) {

    if (error_code.category() == ComCategory()) {
        return SUCCEEDED(error_code.value());
    }
    else {
        return ! error_code;
    }
}


}


#define ZAF_CHECK_ERROR(error_code) ZAF_ASSERT(! zaf::IsSucceeded(error_code))