#pragma once

#include <zaf/base/error/base_runtime_error.h>

namespace zaf {

class InvalidDataError : public BaseRuntimeError {
public:
    using BaseRuntimeError::BaseRuntimeError;
};

}