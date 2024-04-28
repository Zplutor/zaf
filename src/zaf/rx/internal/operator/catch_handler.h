#pragma once

#include <exception>
#include <functional>

namespace zaf::internal {

class InnerObservable;

using CatchHandler = std::function<std::shared_ptr<InnerObservable>(const std::exception_ptr&)>;

}
