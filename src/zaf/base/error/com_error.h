#pragma once

#include <Windows.h>
#include <system_error>
#include <zaf/base/error/system_error.h>

namespace zaf {

class COMError : public std::system_error, public SystemError {
public:
    static const std::error_category& Category();
    static std::error_code MakeCode(HRESULT hresult);

public:
    explicit COMError(HRESULT code) :
        system_error(std::error_code(static_cast<int>(code), Category())) {

    }

    COMError(HRESULT code, const SourceSite& site) :
        system_error(std::error_code(static_cast<int>(code), Category())),
        SystemError(site) {

    }

    const std::error_code& Code() const noexcept override {
        return this->code();
    }

    const char* Message() const noexcept override {
        return this->what();
    }
};


#define ZAF_THROW_COM_ERROR(hresult) throw zaf::COMError{ hresult, ZAF_SOURCE_SITE()}

#define ZAF_THROW_IF_COM_ERROR(hresult) if (FAILED(hresult)) { ZAF_THROW_COM_ERROR(hresult); }

}