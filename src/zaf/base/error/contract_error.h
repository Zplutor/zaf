#pragma once

#include <zaf/base/error/base_logic_error.h>

namespace zaf {

/**
Base class for errors related to contract programming violations.
*/
class ContractError : public BaseLogicError {
public:
    using BaseLogicError::BaseLogicError;
};


/**
Represents an error caused by a failed precondition.
*/
class PreconditionError : public ContractError {
public:
    using ContractError::ContractError;
};


/**
Throws a PreconditionError with the specified message and site.

@param message
    The error message to include in the error.

@param site
    The site where the error occurs.

@details
    This function is internally used by ZAF_EXPECT() to throw PreconditionError, enabling the use 
    of ZAF_EXPECT() in destructors without warning.
*/
[[noreturn]]
inline void ThrowPreconditionError(const char* message, const SourceLocation& site) {
    throw PreconditionError{ message, site };
}


/**
Checks the specified precondition and throws PreconditionError if the condition is not met.

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