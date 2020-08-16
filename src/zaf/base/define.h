#pragma once

#include <cstddef>
#include <limits>
#include <vector>

#undef max

namespace zaf {

const std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

using IndexPath = std::vector<std::size_t>;

}