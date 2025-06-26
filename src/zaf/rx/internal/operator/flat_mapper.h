#pragma once

#include <any>
#include <functional>

namespace zaf::rx::internal {

class ObservableCore;

using FlatMapper = std::function<std::shared_ptr<ObservableCore>(const std::any&)>;

}