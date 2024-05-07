#pragma once

#include <system_error>
#include <zaf/base/error/error.h>

namespace zaf {

class BaseSystemError : public std::system_error, public Error {
public:
    BaseSystemError(int error_value, const std::error_category& category) : 
        system_error(error_value, category) {

    }

    BaseSystemError(
        int error_value,
        const std::error_category& category,
        const SourceLocation& site) 
        :
        system_error(error_value, category),
        Error(site) {

    }

    explicit BaseSystemError(const std::error_code& error_code) : system_error(error_code) {

    }

    BaseSystemError(const std::error_code& error_code, const std::string& message) :
        system_error(error_code, message) {

    }

    BaseSystemError(const std::error_code& error_code, const char* message) :
        system_error(error_code, message) {

    }

    BaseSystemError(const std::error_code& error_code, const SourceLocation& site) :
        system_error(error_code),
        Error(site) {

    }

    BaseSystemError(
        const std::error_code& error_code,
        const std::string& message,
        const SourceLocation& site) 
        :
        system_error(error_code, message),
        Error(site) {

    }

    BaseSystemError(
        const std::error_code& error_code,
        const char* message,
        const SourceLocation& site) 
        :
        system_error(error_code, message),
        Error(site) {

    }

protected:
    ~BaseSystemError() = default;
};

}