#pragma once

#include <Windows.h>
#include <zaf/base/error/base_system_error.h>

namespace zaf {

class Win32Error : public BaseSystemError {
public:
    static const std::error_category& Category();
    static std::error_code MakeCode(DWORD code);

public:
    explicit Win32Error(DWORD error_value) :
        BaseSystemError(static_cast<int>(error_value), Category()) {

    }

    Win32Error(DWORD error_value, const SourceLocation& site) :
        BaseSystemError(static_cast<int>(error_value), Category(), site) {

    }
};


#define ZAF_THROW_WIN32_ERROR(error_code) throw zaf::Win32Error{ error_code, ZAF_SOURCE_LOCATION() }
#define ZAF_THROW_IF_WIN32_ERROR(error_code) if (error_code) { ZAF_THROW_WIN32_ERROR(error_code); }

}