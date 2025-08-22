#pragma once

/**
@file
    Defines class zaf::NotFoundError.
*/

#include <zaf/base/error/runtime_error_base.h>

namespace zaf {

/**
Represents an error indicating that the desired object or data is not found.
*/
class NotFoundError : public RuntimeErrorBase {
public:
    using RuntimeErrorBase::RuntimeErrorBase;
};

}