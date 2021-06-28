#pragma once

#include <type_traits>
#include <zaf/object/boxing/internal/get_box_type.h>
#include <zaf/object/object.h>

namespace zaf::internal {

template<typename T>
struct ObjectUnboxer {

    static const T* Unbox(const Object& object) {
        return dynamic_cast<const T*>(&object);
    }
};


template<typename T>
struct GeneralUnboxer {

    static const T* Unbox(const Object& object) {

        auto boxed_object = dynamic_cast<const typename GetGeneralBoxType<T>::Type*>(&object);
        if (boxed_object) {
            return &boxed_object->GetValue();
        }
        return nullptr;
    }
};


template<typename T>
struct GetUnboxer {
private:
    using DecayType = std::decay_t<T>;

public:
    using Type = std::conditional_t <
        std::is_base_of_v<Object, DecayType>,
        ObjectUnboxer<T>,
        GeneralUnboxer<T>
    >;
};

}