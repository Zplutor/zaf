#pragma once

/**
@file
    Define class zaf::COMError.
*/

#include <Windows.h>
#include <zaf/base/error/base_system_error.h>

namespace zaf {

/**
Represents an error returned by COM interfaces.
*/
class COMError : public BaseSystemError {
public:
    /**
    Gets the error category of COM errors.

    @return
        The error category whose name is `zaf.COM`.
    */
    static const std::error_category& Category();

    /**
    Makes an error code from the specified HRESULT value.

    @param hresult
        The HRESULT value.

    @return
        The error code which has the same category as Category() returns.
    */
    static std::error_code MakeCode(HRESULT hresult);

public:
    /**
    Constructs an instance with the specified HRESULT value.

    @param hresult
        The HRESULT value.

    @throw std::bad_alloc
        Thrown by BaseSystemError if it fails to copy the error message.
    */
    explicit COMError(HRESULT hresult) : BaseSystemError(MakeCode(hresult)) {

    }

    /**
    Constructs an instance with the specified HRESULT value.

    @param hresult
        The HRESULT value.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by BaseSystemError if it fails to copy the error message.
    */
    COMError(HRESULT hresult, const SourceLocation& location) : 
        BaseSystemError(MakeCode(hresult), location) {

    }
};


/**
Throws a zaf::COMError with the specified HRESULT value and the source location where this macro
is invoked.
*/
#define ZAF_THROW_COM_ERROR(hresult) throw zaf::COMError{ hresult, ZAF_SOURCE_LOCATION() }

/**
Checks if the specified HRESULT value is failed and if it is, throws a zaf::COMError with the 
HRESULT value and the source location where this macro is invoked. 
*/
#define ZAF_THROW_IF_COM_ERROR(hresult) if (FAILED(hresult)) { ZAF_THROW_COM_ERROR(hresult); }

}