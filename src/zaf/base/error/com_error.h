#pragma once

#include <Windows.h>
#include <zaf/base/error/base_system_error.h>

namespace zaf {

class COMError : public BaseSystemError {
public:
    static const std::error_category& Category();
    static std::error_code MakeCode(HRESULT hresult);

public:
    explicit COMError(HRESULT hresult) : 
        BaseSystemError(static_cast<int>(hresult), Category()) {

    }

    COMError(HRESULT hresult, const SourceLocation& site) :
        BaseSystemError(static_cast<int>(hresult), Category(), site) {

    }
};


#define ZAF_THROW_COM_ERROR(hresult) throw zaf::COMError{ hresult, ZAF_SOURCE_LOCATION() }

#define ZAF_THROW_IF_COM_ERROR(hresult) if (FAILED(hresult)) { ZAF_THROW_COM_ERROR(hresult); }

}