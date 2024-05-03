#pragma once

#include <system_error>
#include <zaf/base/error/runtime_error.h>

namespace zaf {

class SystemError : public RuntimeError {
public:
    using RuntimeError::RuntimeError;

    virtual const std::error_code& Code() const noexcept = 0;
};  


class GeneralSystemError : public std::system_error, public SystemError {
public:
    explicit GeneralSystemError(const std::error_code& error_code) : system_error(error_code) {

    }

    GeneralSystemError(const std::error_code& error_code, const SourceSite& site) :
        system_error(error_code),
        SystemError(site) {

    }

    const std::error_code& Code() const noexcept override {
        return this->code();
    }

    const char* Message() const noexcept override {
        return this->what();
    }
};

}