#pragma once

#include <Windows.h>
#include <zaf/base/error/system_error.h>

namespace zaf {

class Win32Error : public GeneralSystemError {
public:
    static const std::error_category& Category();
    static std::error_code MakeCode(DWORD code);

public:
    explicit Win32Error(DWORD code) : 
        GeneralSystemError(std::error_code(static_cast<int>(code), Category())) {

    }

    Win32Error(DWORD code, const SourceSite& site) :
        GeneralSystemError(std::error_code(static_cast<int>(code), Category())) {

    }
};


#define ZAF_THROW_WIN32_ERROR(error_code) throw zaf::Win32Error{ error_code, ZAF_SOURCE_SITE() }
#define ZAF_THROW_IF_WIN32_ERROR(error_code) if (error_code) { ZAF_THROW_WIN32_ERROR(error_code); }                            \

}