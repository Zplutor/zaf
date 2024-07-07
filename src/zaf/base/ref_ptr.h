#pragma once

#include <memory>
#include <vector>

namespace zaf {

template<typename T>
using RefPtr = std::shared_ptr<T>;

template<typename T>
using RefPtrVector = std::vector<RefPtr<T>>;

}