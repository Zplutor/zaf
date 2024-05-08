#pragma once

/**
@file
    Defines class zaf::InvalidOperationError.
*/

#include <zaf/base/error/base_runtime_error.h>

namespace zaf {

/**
Represents an error indicating that the operation is unable to be performed.
*/
class InvalidOperationError : public BaseRuntimeError {
public:
    using BaseRuntimeError::BaseRuntimeError;
};

}