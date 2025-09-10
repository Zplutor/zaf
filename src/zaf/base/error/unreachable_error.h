#pragma once

#include <zaf/base/error/logic_error_base.h>

namespace zaf {

/**
Represents an error that a code path thought to be unreachable is actually reached.
*/
class UnreachableError : public LogicErrorBase {
public:
    using LogicErrorBase::LogicErrorBase;
};


/**
Throws a `zaf::UnreachableError` with the specified source location.

@param location
    The source location where the error occurs.

@throw zaf::UnreachableError

@details
    This function is internally used by the `ZAF_UNREACHABLE` macro to throw 
    `zaf::UnreachableError`, enabling the use of `ZAF_UNREACHABLE` in destructors without warning.

@relatesalso zaf::UnreachableError
*/
[[noreturn]]
inline void ThrowUnreachableError(const SourceLocation& location) {
    throw UnreachableError{ location };
}


/**
Marks a code path as unreachable, throws `zaf::UnreachableError` if it is actually reached.

@throw zaf::UnreachableError
*/
#define ZAF_UNREACHABLE() zaf::ThrowUnreachableError(ZAF_SOURCE_LOCATION());

}