#pragma once

#include <zaf/base/error/base_runtime_error.h>

namespace zaf {

class NotFoundError : public BaseRuntimeError {
public:
    using BaseRuntimeError::BaseRuntimeError;
};

}