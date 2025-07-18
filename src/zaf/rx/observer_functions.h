#pragma once

#include <exception>
#include <functional>

namespace zaf::rx {

template<typename T>
using OnNext = std::function<void(const T&)>;

using OnError = std::function<void(const std::exception_ptr&)>;

using OnCompleted = std::function<void()>;

}