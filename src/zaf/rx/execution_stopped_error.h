#pragma once

/**
@file
    Defines the `zaf::rx::ExecutionStoppedError` class.
*/

#include <zaf/base/error/runtime_error_base.h>

namespace zaf::rx {

/**
Represents an error that the execution of a thread or a scheduler is stopped.
*/
class ExecutionStoppedError : public RuntimeErrorBase {
public:
    using RuntimeErrorBase::RuntimeErrorBase;
};

}