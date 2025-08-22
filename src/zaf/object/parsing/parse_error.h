#pragma once

#include <zaf/base/error/runtime_error_base.h>

namespace zaf {

class ParseError : public RuntimeErrorBase {
public:
    using RuntimeErrorBase::RuntimeErrorBase;
};

}