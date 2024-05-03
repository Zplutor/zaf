#pragma once

#include <zaf/base/error/runtime_error.h>

namespace zaf {

class NotFoundError : public GeneralRuntimeError {
public:
    using GeneralRuntimeError::GeneralRuntimeError;
};

}