#pragma once

/**
@file
    Defines class zaf::NotFoundError.
*/

#include <zaf/base/error/base_runtime_error.h>

namespace zaf {

/**
Represents an error indicating that the desired object or data is not found.
*/
class NotFoundError : public BaseRuntimeError {
public:
    using BaseRuntimeError::BaseRuntimeError;
};

}