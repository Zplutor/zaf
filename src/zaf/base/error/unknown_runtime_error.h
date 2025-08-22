#pragma once

/**
@file
    Defines class zaf::UnknownRuntimeError.
*/

#include <zaf/base/error/runtime_error_base.h>

namespace zaf {

/**
Represents a runtime error caused by an unknown reason.

@details
    Users should consider using other concrete exception classes before resorting 
    UnknownRuntimeError, which should be the last choice if there is indeed no suitable exception 
    class available.
*/
class UnknownRuntimeError : public RuntimeErrorBase {
public:
    using RuntimeErrorBase::RuntimeErrorBase;
};

}