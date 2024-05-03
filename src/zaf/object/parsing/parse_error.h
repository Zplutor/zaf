#pragma once

#include <zaf/base/error/runtime_error.h>

namespace zaf {

class ParseError : public GeneralRuntimeError {
public:
    using GeneralRuntimeError::GeneralRuntimeError;
};

}