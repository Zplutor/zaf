#pragma once

#include <zaf/base/error/runtime_error.h>

namespace zaf {

class InvalidOperationError : public GeneralRuntimeError {
public:
    using GeneralRuntimeError::GeneralRuntimeError;
};

}