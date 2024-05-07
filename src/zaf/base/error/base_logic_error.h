#pragma once

/**
@file
Defines class zaf::BaseLogicError.
*/

#include <stdexcept>
#include <zaf/base/error/error.h>

namespace zaf {

/**
A pre-defined base class that inherits from both std::logic_error and Error, providing a convenient
way to define concrete exception classes related to logic errors.

@details
    To define a concrete logic error exception class, users can follow the code shown below:

    @code{.cpp}
    class MyLogicError : public BaseLogicError {
    public:
        //Instroduces various constructors from BaseLogicError.
        using BaseLogicError::BaseLogicError;
    };
    @endcode
*/
class BaseLogicError : public std::logic_error, public Error {
public:
    /**
    Constructs an instance without any error information.
    */
    BaseLogicError() noexcept : logic_error(nullptr) {

    }

    /**
    Constructs an instance with the specified error message.

    @param message
        The error message that describes the error.

    @throw std::bad_alloc
        Thrown by std::logic_error if it fails to copy the message.
    */
    explicit BaseLogicError(const std::string& message) : logic_error(message) {

    }

    /**
    Constructs an instance with the specified error message.

    @param message
        The error message that describes the error. Can be null.

    @throw std::bad_alloc
        Thrown by std::logic_error if it fails to copy the message.
    */
    explicit BaseLogicError(const char* message) : logic_error(message) {

    }

    /**
    Constructs an instance with the specified source location.

    @param location
        The source location where the error occurs.
    */
    explicit BaseLogicError(const SourceLocation& location) noexcept : 
        logic_error(nullptr), 
        Error(location) {

    }

    /**
    Constructs an instance with the specified error message and source location.

    @param message
        The error message that describes the error.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by std::logic_error if it fails to copy the message.
    */
    BaseLogicError(const std::string& message, const SourceLocation& location) :
        logic_error(message), 
        Error(location) {

    }

    /**
    Constructs an instance with the specified error message and source location.

    @param message
        The error message that describes the error. Can be null.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by std::logic_error if it fails to copy the message.
    */
    BaseLogicError(const char* message, const SourceLocation& location) :
        logic_error(message),
        Error(location) {

    }

protected:
    ~BaseLogicError() = default;
};

}