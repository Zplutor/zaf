#pragma once

/**
@file
    Defines class zaf::InvalidDataError.
*/

#include <zaf/base/error/runtime_error_base.h>

namespace zaf {

/**
Represents an error caused by invalid data.
*/
class InvalidDataError : public RuntimeErrorBase {
public:
    using RuntimeErrorBase::RuntimeErrorBase;
};

}