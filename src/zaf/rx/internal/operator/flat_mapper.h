#pragma once

#include <any>
#include <functional>

namespace zaf::internal {

class InnerObservable;

using FlatMapper = std::function<std::shared_ptr<InnerObservable>(const std::any&)>;

}