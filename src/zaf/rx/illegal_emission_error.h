#pragma once

#include <zaf/base/error/contract_error.h>

namespace zaf::rx {

/**
Represents an error caused by an illegal emission of an observable, such as emitting a value after
the observable has terminated.
*/
class IllegalEmissionError : public PreconditionError {
public:
    using PreconditionError::PreconditionError;
};

}