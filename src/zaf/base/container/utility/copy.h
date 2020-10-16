#pragma once

#include <algorithm>

namespace zaf {

template<typename C, typename P>
C CopyIf(const C& container, P& predicate) {

    C result;
    std::copy_if(
        container.begin(), container.end(), 
        std::inserter(result, result.end()),
        std::forward<P>(predicate));

    return result;
}

}