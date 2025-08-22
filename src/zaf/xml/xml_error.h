#pragma once

/**
@file
    Defines the class zaf::XMLError.
*/

#include <zaf/base/error/runtime_error_base.h>

namespace zaf {

class XMLError : public RuntimeErrorBase {
public:
    using RuntimeErrorBase::RuntimeErrorBase;
};

}