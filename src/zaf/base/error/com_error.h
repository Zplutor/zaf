#pragma once

#include <Windows.h>
#include <zaf/base/error/base_system_error.h>

namespace zaf {

class COMError : public BaseSystemError {
public:
    static const std::error_category& Category();
    static std::error_code MakeCode(HRESULT hresult);

public:
    COMError(HRESULT code, const SourceSite& site) :
        BaseSystemError(std::error_code(static_cast<int>(code), Category()), site) {

    }
};


#define ZAF_THROW_COM_ERROR(hresult) throw zaf::COMError{ hresult, ZAF_SOURCE_SITE() }

#define ZAF_THROW_IF_COM_ERROR(hresult) if (FAILED(hresult)) { ZAF_THROW_COM_ERROR(hresult); }

}