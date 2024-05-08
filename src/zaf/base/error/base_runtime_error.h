#pragma once

/**
@file
    Defines class zaf::BaseRuntimeError.
*/

#include <stdexcept>
#include <zaf/base/error/error.h>

namespace zaf {

/**
A pre-defined base class that inherits from both std::runtime_error and Error, providing a 
convenient way to define concrete exception classes related to runtime errors.

@details
    To define a concrete runtime error exception class, users can follow the code shown below:

    @code{.cpp}
    class MyRuntimeError : public BaseRuntimeError {
    public:
        //Instroduces various constructors from BaseRuntimeError.
        using BaseRuntimeError::BaseRuntimeError;
    };
    @endcode

    BaseRuntimeError is designed for inheritance purposes only, and it is not allow to be
    instantiated directly.
*/
class BaseRuntimeError : public std::runtime_error, public Error {
public:
    /**
    Constructs an instance without any error information.
    */
    BaseRuntimeError() noexcept : runtime_error(nullptr) {

    }

    /**
    Constructs an instance with the specified error message.

    @param message
        The error message that describes the error.

    @throw std::bad_alloc
        Thrown by std::runtime_error if it fails to copy the message.
    */
    explicit BaseRuntimeError(const std::string& message) : runtime_error(message) {

    }

    /**
    Constructs an instance with the specified error message.

    @param message
        The error message that describes the error. Can be null.

    @throw std::bad_alloc
        Thrown by std::runtime_error if it fails to copy the message.
    */
    explicit BaseRuntimeError(const char* message) : runtime_error(message) {

    }

    /**
    Constructs an instance with the specified source location.

    @param location
        The source location where the error occurs.
    */
    explicit BaseRuntimeError(const SourceLocation& location) noexcept : 
        runtime_error(nullptr), 
        Error(location) {

    }

    /**
    Constructs an instance with the specified error message and source location.

    @param message
        The error message that describes the error.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by std::runtime_error if it fails to copy the message.
    */
    BaseRuntimeError(const std::string& message, const SourceLocation& location) :
        runtime_error(message),
        Error(location) {

    }

    /**
    Constructs an instance with the specified error message and source location.

    @param message
        The error message that describes the error. Can be null.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by std::runtime_error if it fails to copy the message.
    */
    BaseRuntimeError(const char* message, const SourceLocation& location) :
        runtime_error(message),
        Error(location) {

    }

protected:
    ~BaseRuntimeError() = default;
};

}