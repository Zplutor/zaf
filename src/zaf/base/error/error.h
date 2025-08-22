#pragma once

/**
@file
    Defines the `zaf::Error` class.
*/

#include <zaf/base/source_location.h>

namespace zaf {

/**
`zaf::Error` is a mixin base class for all exceptions in the ZAF library, provides more information 
about the error than standard exceptions, such as the source location where the error occurs.

@details
    `zaf::Error` is an extension to standard exceptions. It does not inherit from any standard 
    exception. Instead, derived classes inherit from both standard exception and `zaf::Error` to 
    remain compatible with standard exception handling and gain the extended ability provided by
    `zaf::Error`.

    There are several pre-defined exception classes in ZAF already inherit from standard exceptions
    and `zaf::Error`: `zaf::LogicErrorBase`, `zaf::RuntimeErrorBase` and `zaf::SystemErrorBase`. 
    Users can conveniently define new exception classes by inheriting from these classes.
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
        The source location is not mandatory to be provided when creating `zaf::Error` instances 
        and might be left empty.

    @note
        Exceptions thrown by the ZAF library are guaranteed to have a source location.
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