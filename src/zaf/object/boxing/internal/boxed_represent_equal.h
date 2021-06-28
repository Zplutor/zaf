#pragma once

#include <zaf/object/object.h>

namespace zaf::internal {

template<typename T>
bool BoxedRepresentEqual(const T& boxed, const Object& other) {

    auto other_boxed = dynamic_cast<const T*>(&other);
    if (other_boxed) {
        return boxed.GetValue() == other_boxed->GetValue();
    }
    return false;
}

}