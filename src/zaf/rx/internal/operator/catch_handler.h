#pragma once

#include <exception>
#include <functional>

namespace zaf::internal {

class ObservableCore;

using CatchHandler = std::function<std::shared_ptr<ObservableCore>(const std::exception_ptr&)>;

}
