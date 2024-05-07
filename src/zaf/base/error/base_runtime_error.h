#pragma once

#include <stdexcept>
#include <zaf/base/error/error.h>

namespace zaf {

class BaseRuntimeError : public std::runtime_error, public Error {
public:
    BaseRuntimeError() : runtime_error("") {

    }

    explicit BaseRuntimeError(const std::string& message) : runtime_error(message) {

    }

    explicit BaseRuntimeError(const char* message) : runtime_error(message) {

    }

    explicit BaseRuntimeError(const SourceLocation& site) : runtime_error(""), Error(site) {

    }

    BaseRuntimeError(const std::string& message, const SourceLocation& site) :
        runtime_error(message),
        Error(site) {

    }

    BaseRuntimeError(const char* message, const SourceLocation& site) :
        runtime_error(message),
        Error(site) {

    }

protected:
    ~BaseRuntimeError() = default;
};

}