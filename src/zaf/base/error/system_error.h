#pragma once

#include <system_error>
#include <zaf/base/error/runtime_error.h>

namespace zaf {

class SystemError : public RuntimeError {
public:
    using RuntimeError::RuntimeError;

    virtual const std::error_code& Code() const noexcept = 0;
};  

}