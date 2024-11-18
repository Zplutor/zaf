#include <zaf/internal/tree/utility.h>
#include <zaf/base/range.h>

namespace zaf::internal {

bool IsAncestorOf(const IndexPath& ancestor, const IndexPath& path) {

    if (ancestor.size() >= path.size()) {
        return false;
    }

    for (auto index : Range(0, ancestor.size())) {

        if (ancestor[index] != path[index]) {
            return false;
        }
    }

    return true;
}

}