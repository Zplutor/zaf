#pragma once

#include <stdexcept>

namespace zaf {

#define ZAF_NOT_REACHED() throw std::logic_error{ "not reached" };

#define ZAF_ALERT() assert(false);

}