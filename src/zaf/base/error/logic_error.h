#pragma once

#include <zaf/base/error/error.h>

namespace zaf {

/**
Base class for all logic errors in the zaf library.
*/
class LogicError : public Error {
public:
    using Error::Error;
};

}