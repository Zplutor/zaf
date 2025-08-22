#pragma once

/**
@file
    Defines the `zaf::Win32Error` class.
*/

#include <Windows.h>
#include <zaf/base/error/system_error_base.h>

namespace zaf {

/**
Represents an error returned by Win32 APIs.

@details
    The error value of a `zaf::Win32Error` is typically the return value of GetLastError().
*/
class Win32Error : public SystemErrorBase {
public:
    /**
    Gets the error category of Win32 errors.

    @return
        The error category whose name is `ZAF.Win32`.
    */
    static const std::error_category& Category();

    /**
    Makes an error code from the specified Win32 error value.

    @param value
        The Win32 error value.

    @return
        The error code which has the same category as Category() returns.
    */
    static std::error_code MakeCode(DWORD value);

public:
    /**
    Constructs an instance with the specified Win32 error value.

    @param error_value
        The Win32 error value.

    @throw std::bad_alloc
        Thrown by `zaf::SystemErrorBase` if it fails to copy the error message.
    */
    explicit Win32Error(DWORD error_value) : SystemErrorBase(MakeCode(error_value)) {

    }

    /**
    Constructs an instance with the specified Win32 error value and source location.

    @param error_value
        The Win32 error value.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by `zaf::SystemErrorBase` if it fails to copy the error message.
    */
    Win32Error(DWORD error_value, const SourceLocation& location) :
        SystemErrorBase(MakeCode(error_value), location) {

    }
};


/**
Throws a `zaf::Win32Error` with the specified Win32 error value and the source location where this 
macro is invoked.
*/
#define ZAF_THROW_WIN32_ERROR(value) throw zaf::Win32Error{ value, ZAF_SOURCE_LOCATION() }

/**
Checks if the specified Win32 error value is failed and if it is, throws a `zaf::Win32Error` with 
the error value and the source location where this macro is invoked.
*/
#define ZAF_THROW_IF_WIN32_ERROR(value) if (value) { ZAF_THROW_WIN32_ERROR(value); }

}