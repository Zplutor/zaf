#pragma once

/**
@file
    Defines class zaf::InvalidOperationError.
*/

#include <zaf/base/error/runtime_error_base.h>

namespace zaf {

/**
Represents an error indicating that the operation is unable to be performed.
*/
class InvalidOperationError : public RuntimeErrorBase {
public:
    using RuntimeErrorBase::RuntimeErrorBase;
};

}