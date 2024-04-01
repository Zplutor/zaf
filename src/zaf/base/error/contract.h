#pragma once

#include <stdexcept>

namespace zaf {

/**
Base class for errors related to contract programming violations.
*/
class ContractError : public std::logic_error {
protected:
    using logic_error::logic_error;
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
    throw PreconditionError{ message };
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