#pragma once

namespace zaf {

template<typename C>
const typename C::mapped_type* Find(const C& container, const typename C::key_type& key) {
    auto iterator = container.find(key);
    if (iterator != container.end()) {
        return &iterator->second;
    }
    return nullptr;
}

template<typename C>
typename C::mapped_type* Find(C& container, const typename C::key_type& key) {
    return const_cast<typename C::mapped_type*>(Find(static_cast<const C&>(container), key));
}

}
