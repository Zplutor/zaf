#pragma once

/**
@file
    Defines the `zaf::PreconditionError` class and the related functions and macros.
*/

#include <zaf/base/error/contract_error.h>

namespace zaf {

/**
Represents an error caused by a failed precondition.
*/
class PreconditionError : public ContractError {
public:
    using ContractError::ContractError;
};


/**
Throws a `zaf::PreconditionError` with the specified message and source location.

@param message
    The error message to include in the error.

@param location
    The source location where the error occurs.

@details
    This function is internally used by the `ZAF_EXPECT` macro to throw `zaf::PreconditionError`, 
    enabling the use of `ZAF_EXPECT` in destructors without warning.

@relatesalso zaf::PreconditionError
*/
[[noreturn]]
inline void ThrowPreconditionError(const char* message, const SourceLocation& location) {
    throw PreconditionError{ message, location };
}


/**
Checks the specified precondition and throws `zaf::PreconditionError` if the condition is not met.

@param precondition
    The condition to check. It must be convertible to bool.

@throw zaf::PreconditionError
    Thrown if the condition is not met.

@details
    This macro is intended to be used for enforcing preconditions in contract programming.
*/
#define ZAF_EXPECT(precondition) \
if (!(precondition)) zaf::ThrowPreconditionError(#precondition, ZAF_SOURCE_LOCATION());

}