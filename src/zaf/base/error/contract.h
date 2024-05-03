#pragma once

#include <stdexcept>
#include <zaf/base/error/logic_error.h>

namespace zaf {

/**
Base class for errors related to contract programming violations.
*/
class ContractError : public GeneralLogicError {
public:
    using GeneralLogicError::GeneralLogicError;
};


/**
Represents an error caused by a failed precondition.
*/
class PreconditionError : public ContractError {
public:
    using ContractError::ContractError;
};


/**
Throws a PreconditionError with the specified message.

@param message
    The error message to include in the error.

@remark
    This function is internally used by ZAF_EXPECT() to throw PreconditionError.
*/
[[noreturn]]
inline void ThrowPreconditionError(const char* message) {
    throw PreconditionError{ message, ZAF_SOURCE_SITE() };
}


/**
Checks the specified precondition and throws PreconditionError if the condition is not met.

@param precondition
    The condition to check. It must be convertable to bool.

@throw zaf::PreconditionError
    Thrown if the condition is not met.

@remark
    This macro is intended to be used for enforcing preconditions in contract programming.
*/
#define ZAF_EXPECT(precondition) if (!(precondition)) zaf::ThrowPreconditionError(#precondition);

}