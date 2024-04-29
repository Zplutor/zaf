#pragma once

#include <Windows.h>
#include <system_error>
#include <zaf/base/error/runtime_error.h>

namespace zaf {

class SystemError : public std::system_error, public RuntimeError {
public:
    SystemError(const std::error_code& code) : system_error(code) {
    
    }

    SystemError(const std::error_code& code, const std::string& message);

    const char* Message() const override {
        return this->what();
    }
};


inline std::error_code MakeSystemErrorCode(DWORD code) {
    return std::error_code{ static_cast<int>(code), std::system_category() };
}


#define ZAF_THROW_SYSTEM_ERROR(error_code) \
throw zaf::Error{ zaf::MakeSystemErrorCode(error_code), ZAF_SOURCE_SITE() }


#define ZAF_THROW_IF_SYSTEM_ERROR(error_code)                            \
if (error_code != 0) {                                                   \
    ZAF_THROW_SYSTEM_ERROR(error_code);                                  \
}
  

}