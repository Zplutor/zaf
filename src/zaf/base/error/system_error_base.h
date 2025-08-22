#pragma once

/**
@file 
    Defines the `zaf::SystemErrorBase` class.
*/

#include <system_error>
#include <zaf/base/error/error.h>

namespace zaf {

/**
A pre-defined base class that inherits from both `std::system_error` and `zaf::Error`, providing a
convenient way to define concrete exception classes related to system errors.

@details
    To define a concrete system error exception class, users can follow the code shown below:

    @code{.cpp}
    //Gets the user-defined error category.
    const std::error_category& MyErrorCategory();

    class MySystemError : public zaf::SystemErrorBase {
    public:
        //Constructs an instance with the specified error value in the user-defined error catgory.
        explicit MySystemError(int my_error_value) : 
            SystemErrorBase(my_error_value, MyErrorCategory()) {

        }

        //Define more constructors as needed.
    };
    @endcode

    `zaf::SystemErrorBase` is designed for inheritance purposes only, and it is not allow to be 
    instantiated directly.

@note
    `zaf::SystemErrorBase` does not inherit from `zaf::RuntimeErrorBase`. Users should use 
    `std::runtime_error` in catch statements to catch exceptions inherit from either 
    `zaf::RuntimeErrorBase` or `zaf::SystemErrorBase`. 
*/
class SystemErrorBase : public std::system_error, public Error {
public:
    /**
    Constructs an instance with the specified error value and error category.

    @param error_value
        The error value.

    @param category
        The error category to which the error value belongs.

    @throw std::bad_alloc
        Thrown by std::system_error if it fails to copy the message returned by the error category.
    */
    SystemErrorBase(int error_value, const std::error_category& category) : 
        system_error(error_value, category) {

    }

    /**
    Constructs an instance with the specified error value, error category and error message.

    @param error_value
        The error value.

    @param category
        The error category to which the error value belongs.

    @param message
        The error message that describes the error.

    @throw std::bad_alloc
        Thrown by std::system_error if it fails to copy the error message or the message returned 
        by the error category..
    */
    SystemErrorBase(
        int error_value, 
        const std::error_category& category,
        const std::string& message) 
        :
        system_error(error_value, category, message) {

    }

    /**
    Constructs an instance with the specified error value, error category and source location.

    @param error_value
        The error value.

    @param category
        The error category to which the error value belongs.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by std::system_error if it fails to copy the message returned by the error category.
    */
    SystemErrorBase(
        int error_value,
        const std::error_category& category,
        const SourceLocation& location) 
        :
        system_error(error_value, category),
        Error(location) {

    }

    /**
    Constructs an instance with the specified error value, error category, error message and source 
    location.

    @param error_value
        The error value.

    @param category
        The error category to which the error value belongs.

    @param message
        The error message that describes the error.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by std::system_error if it fails to copy the error message or the message returned 
        by the error category.
    */
    SystemErrorBase(
        int error_value,
        const std::error_category& category,
        const std::string& message,
        const SourceLocation& location)
        :
        system_error(error_value, category, message),
        Error(location) {

    }

    /**
    Constructs an instance with the specified error code.

    @param error_code
        The error code.

    @throw std::bad_alloc
        Thrown by std::system_error if it fails to copy the message returned by the error code.
    */
    explicit SystemErrorBase(const std::error_code& error_code) : system_error(error_code) {

    }

    /**
    Constructs an instance with the specified error code and error message.

    @param error_code
        The error code.

    @param message
        The error message that describes the error.

    @throw std::bad_alloc
        Thrown by std::system_error if it fails to copy the error message or the message returned
        by the error code.
    */
    SystemErrorBase(const std::error_code& error_code, const std::string& message) :
        system_error(error_code, message) {

    }

    /**
    Constructs an instance with the specified error code and source location.

    @param error_code
        The error code.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by std::system_error if it fails to copy the message returned by the error code.
    */
    SystemErrorBase(const std::error_code& error_code, const SourceLocation& location) :
        system_error(error_code),
        Error(location) {

    }

    /**
    Constructs an instance with the specified error code, error message and source location.

    @param error_code
        The error code.

    @param message
        The error message that describes the error.

    @param location
        The source location where the error occurs.

    @throw std::bad_alloc
        Thrown by std::system_error if it fails to copy the error message or the message returned
        by the error code.
    */
    SystemErrorBase(
        const std::error_code& error_code,
        const std::string& message,
        const SourceLocation& location)
        :
        system_error(error_code, message),
        Error(location) {

    }

protected:
    ~SystemErrorBase() = default;
};

}