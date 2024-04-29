#pragma once

#include <zaf/base/error/error.h>

namespace zaf {

/**
Base class for all runtime errors in the zaf library.
*/
class RuntimeError : public Error {
public:
    using Error::Error;

    virtual const char* Message() const = 0;
};

}