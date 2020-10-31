#pragma once

#include <stdexcept>

namespace zaf {

#define ZAF_CHECK(expression) \
if (!(expression)) throw std::logic_error{ #expression };

#define ZAF_EXPECT(expression) \
if (!(expression)) throw std::logic_error{ #expression };

}