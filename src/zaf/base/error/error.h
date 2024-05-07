#pragma once

/**
@file
Defines class zaf::Error.
*/

#include <zaf/base/source_location.h>

namespace zaf {

/**
Base class for all error exceptions in the zaf library, provides more information about the error 
than standard exceptions, such as the source location where the error occurs.

@details
    Error is an extension to standard exceptions. It does not inherit from any standard exception. 
    Instead, derived classes inherit from both standard exception and Error to remain compatible
    with standard exception handling and gain the extended ability provided by Error.

    Some pre-defined exception classes in zaf already inherit from standard exceptions and Error:
    BaseLogicError, BaseRuntimeError and BaseSystemError. Users can conveniently define new 
    exception classes by inheriting from these classes.
*/
class Error {
public:
    /**
    Constructs an instance without any information.
    */
    Error() noexcept = default;

    /**
    Constructs an instance with the specified source location where the error occurs.
    */
    explicit Error(const SourceLocation& location) noexcept : location_(location) { }

    /**
    Gets the source location where the error occurs.

    @return
        The source location where the error occurs.

    @note
        The source location is not mandatory to be provided when creating Error instances and 
        might be left empty. 
        
    @note
        Exceptions thrown by the zaf library are guranteed to have a source location.
    */
    const SourceLocation& Location() const noexcept {
        return location_;
    }

protected:
    virtual ~Error() = default;

private:
    SourceLocation location_;
};

}