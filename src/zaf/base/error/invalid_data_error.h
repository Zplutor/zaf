#pragma once

/**
@file
    Defines class zaf::InvalidDataError.
*/

#include <zaf/base/error/base_runtime_error.h>

namespace zaf {

/**
Represents an error caused by invalid data.
*/
class InvalidDataError : public BaseRuntimeError {
public:
    using BaseRuntimeError::BaseRuntimeError;
};

}