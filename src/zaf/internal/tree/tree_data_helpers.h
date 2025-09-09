#pragma once

#include <zaf/dynamic/object.h>

namespace zaf::internal {

struct TreeDataHash {
    std::size_t operator()(const std::shared_ptr<Object>& object) const {
        if (object) {
            return object->Hash();
        }
        return 0;
    }
};

struct TreeDataEqual {
    bool operator()(const std::shared_ptr<Object>& o1, const std::shared_ptr<Object>& o2) const {

        if (!o1 && !o2) {
            return true;
        }

        if (!o1 || !o2) {
            return false;
        }

        return o1->IsEqual(*o2);
    }
};

}