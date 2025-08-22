#pragma once

/**
@file 
Defines classes, functions and macros related to contract programming.
*/

#include <zaf/base/error/logic_error_base.h>

namespace zaf {

/**
Base class for errors related to contract programming violations.
*/
class ContractError : public LogicErrorBase {
public:
    using LogicErrorBase::LogicErrorBase;
};


/**
Represents an error caused by a failed precondition.
*/
class PreconditionError : public ContractError {
public:
    using ContractError::ContractError;
};


/**
Throws a zaf::PreconditionError with the specified message and source location.

@param message
    The error message to include in the error.

@param location
    The source location where the error occurs.

@details
    This function is internally used by ZAF_EXPECT() to throw zaf::PreconditionError, enabling the
    use of ZAF_EXPECT() in destructors without warning.

@relatesalso PreconditionError
*/
[[noreturn]]
inline void ThrowPreconditionError(const char* message, const SourceLocation& location) {
    throw PreconditionError{ message, location };
}


/**
Checks the specified precondition and throws zaf::PreconditionError if the condition is not met.

@param precondition
    The condition to check. It must be convertable to bool.

@throw zaf::PreconditionError
    Thrown if the condition is not met.

@details
    This macro is intended to be used for enforcing preconditions in contract programming.
*/
#define ZAF_EXPECT(precondition) \
if (!(precondition)) zaf::ThrowPreconditionError(#precondition, ZAF_SOURCE_LOCATION());

}