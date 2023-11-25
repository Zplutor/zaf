#pragma once

#include <any>
#include <functional>

namespace zaf::internal {

using Mapper = std::function<std::any(const std::any&)>;

}