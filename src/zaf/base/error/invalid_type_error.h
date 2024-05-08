#pragma once

/**
@file
    Defines class zaf::InvalidTypeError.
*/

#include <zaf/base/error/base_runtime_error.h>

namespace zaf {

/**
Represents an error related to dynamic type cast failure.
*/
class InvalidTypeError : public BaseRuntimeError {
public:
    using BaseRuntimeError::BaseRuntimeError;
};

}