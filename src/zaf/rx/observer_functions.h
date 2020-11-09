#pragma once

#include <functional>
#include <zaf/base/error/error.h>

namespace zaf {

template<typename T>
using OnNext = std::function<void(const T&)>;

using OnError = std::function<void(const Error&)>;

using OnCompleted = std::function<void()>;

}