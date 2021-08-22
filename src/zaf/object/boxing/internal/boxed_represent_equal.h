#pragma once

#include <zaf/object/object.h>

namespace zaf::internal {

template<typename T>
bool BoxedRepresentEqual(const T& boxed, const Object& other) {

    auto other_boxed = dynamic_cast<const T*>(&other);
    if (other_boxed) {
        return boxed.Value() == other_boxed->Value();
    }
    return false;
}

}