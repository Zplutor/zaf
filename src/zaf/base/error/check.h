#pragma once

#include <stdexcept>

namespace zaf {
namespace internal {

[[noreturn]]
inline void ThrowLogicError(const char* message) {
    throw std::logic_error{ message };
}

}

#define ZAF_EXPECT(expression) if (!(expression)) zaf::internal::ThrowLogicError(#expression);

#define ZAF_NOT_REACHED() throw std::logic_error{ "not reached" };

#define ZAF_ALERT() assert(false);

}