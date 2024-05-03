#pragma once

#include <zaf/base/error/runtime_error.h>

namespace zaf {

class InvalidURIError : public GeneralRuntimeError {
public:
    using GeneralRuntimeError::GeneralRuntimeError;
};

}