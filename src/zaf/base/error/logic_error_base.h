#pragma once

/**
@file
    Defines the `zaf::LogicErrorBase` class.
*/

#include <stdexcept>
#include <zaf/base/error/error.h>

namespace zaf {

/**
A pre-defined base class that inherits from both `std::logic_error` and `zaf::Error`, providing a 
convenient way to define concrete exception classes related to logic errors.

@details
    To define a concrete logic error exception class, users can follow the code shown below:

    @code{.cpp}
    class MyLogicError : public zaf::LogicErrorBase {
    public:
        //Introduces various constructors from zaf::LogicErrorBase.
        using LogicErrorBase::LogicErrorBase;
    };
    @endcode

    `zaf::LogicErrorBase` is designed for inheritance purposes only, and it is not allow to be 
    instantiated directly.
*/
class LogicErrorBase : public std::logic_error, public Error {
public:
    /**
    Constructs an instance without any error information.
    */
    LogicErrorBase() noexcept : logic_error(nullptr) {

    }

    /**
    Constructs an instance with the specified error message.

    @param message
        The error message that describes the error.

    @throw std::bad_alloc
        Thrown by `std::logic_error` if it fails to copy the message.
    */
    explicit LogicErrorBase(const std::string& message) : logic_error(message) {

    }

    /**
    Constructs an instance with the specified error message.

    @param message
        The error message that describes the error. Can be null.

    @throw std::bad_alloc
        Thrown by `std::logic_error` if it fails to copy the message.
    */
    explicit LogicErrorBase(const char* message) : logic_error(message) {

    }

    /**
    Constructs an instance with the specified source location.

    @param location
        The source location where the error occurs.
    */
    explicit LogicErrorBase(const SourceLocation& location) noexcept : 
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
        Thrown by `std::logic_error` if it fails to copy the message.
    */
    LogicErrorBase(const std::string& message, const SourceLocation& location) :
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
        Thrown by `std::logic_error` if it fails to copy the message.
    */
    LogicErrorBase(const char* message, const SourceLocation& location) :
        logic_error(message),
        Error(location) {

    }

protected:
    ~LogicErrorBase() = default;
};

}