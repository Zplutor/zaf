#pragma once

#include <any>
#include <functional>

namespace zaf::rx::internal {

using Mapper = std::function<std::any(const std::any&)>;

}