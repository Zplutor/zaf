#pragma once

/**
@file
    Defines the class zaf::XMLError.
*/

#include <zaf/base/error/base_runtime_error.h>

namespace zaf {

class XMLError : public BaseRuntimeError {
public:
    using BaseRuntimeError::BaseRuntimeError;
};

}