#pragma once

/**
@file
    Defines class zaf::NTError.
*/

#include <zaf/base/error/system_error_base.h>

namespace zaf {

/**
Represents an error corresponding to NTSTATUS.
*/
class NTError : public SystemErrorBase {
public:
    /**
    Gets the error category of NTSTATUS errors.

    @return
        The error category whose name is `zaf.NT`.
    */
    static const std::error_category& Category();

    /**
    Makes an error code from the specified NTSTATUS value.

    @param value
        The NTSTATUS value.

    @return
        The error code which has the same category as Category() returns.
    */
    static std::error_code MakeCode(NTSTATUS value);

public:
    /**
    Constructs an instance with the specified NTSTATUS value.

    @param error_value
        The NTSTATUS value.

    @throw std::bad_alloc
        Thrown by SystemErrorBase if it fails to copy the error message.
    */
    explicit NTError(NTSTATUS error_value) : SystemErrorBase(MakeCode(error_value)) {

    }

    /**
    Constructs an instance with the specified NTSTATUS value and source location.

    @param error_value
        The NTSTATUS value.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by SystemErrorBase if it fails to copy the error message.
    */
    NTError(NTSTATUS error_value, const SourceLocation& location) :
        SystemErrorBase(MakeCode(error_value), location) {

    }
};

}