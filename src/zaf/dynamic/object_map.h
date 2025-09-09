#pragma once

#include <unordered_map>
#include <zaf/dynamic/object.h>

namespace zaf {
namespace internal {

struct ObjectHash {

    using is_transparent = void;

    std::size_t operator()(const std::shared_ptr<const dynamic::Object>& object) const {
        return object->Hash();
    }

    std::size_t operator()(const dynamic::Object& object) const {
        return object.Hash();
    }
};

struct ObjectEqual {

    using is_transparent = void;

    bool operator()(
        const std::shared_ptr<const dynamic::Object>& object1,
        const std::shared_ptr<const dynamic::Object>& object2) const {
        return object1->IsEqual(*object2);
    }

    bool operator()(
        const dynamic::Object& object1, 
        const std::shared_ptr<const dynamic::Object>& object2) const {
        return object1.IsEqual(*object2);
    }
};

}

template<typename T>
using ObjectMap = std::unordered_map<
    std::shared_ptr<dynamic::Object>,
    T,
    internal::ObjectHash, 
    internal::ObjectEqual
>;

}