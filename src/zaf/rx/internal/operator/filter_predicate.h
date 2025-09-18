#pragma once

#include <any>
#include <functional>

namespace zaf::rx::internal {

using FilterPredicate = std::function<bool(const std::any&)>;

}
