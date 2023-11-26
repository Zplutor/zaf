#pragma once

#include <functional>
#include <zaf/base/error/error.h>

namespace zaf::internal {

class InnerObservable;

using CatchHandler = std::function<std::shared_ptr<InnerObservable>(const Error&)>;

}
