#pragma once

#include <zaf/base/error/runtime_error.h>

namespace zaf {

class InvalidDataError : public GeneralRuntimeError {
public:
    using GeneralRuntimeError::GeneralRuntimeError;
};

}