#pragma once

#include <zaf/base/error/runtime_error.h>

namespace zaf {

class NotSupportedError : public GeneralRuntimeError {
public:
    using GeneralRuntimeError::GeneralRuntimeError;
};

}