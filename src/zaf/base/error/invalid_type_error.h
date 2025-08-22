#pragma once

/**
@file
    Defines class zaf::InvalidTypeError.
*/

#include <zaf/base/error/runtime_error_base.h>

namespace zaf {

/**
Represents an error related to dynamic type cast failure.
*/
class InvalidTypeError : public RuntimeErrorBase {
public:
    using RuntimeErrorBase::RuntimeErrorBase;
};

}