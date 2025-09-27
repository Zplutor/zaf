#pragma once

/**
@file
    Defines the `zaf::InvalidHandleStateError` class.
*/

#include <zaf/base/error/invalid_operation_error.h>

namespace zaf {

/**
Represents an error indicating that the window handle is in an invalid state for the operation.
*/
class InvalidHandleStateError : public zaf::InvalidOperationError {
public:
    using InvalidOperationError::InvalidOperationError;
};

}