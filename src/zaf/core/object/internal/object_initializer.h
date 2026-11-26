#pragma once

#include <type_traits>

namespace zaf {
class Object;
}

namespace zaf::internal {

class ObjectInitializer {
public:
    static void Initialize(Object* object);

private:
    ObjectInitializer() = delete;
};


class DumbInitializer {
public:
    static void Initialize(void*) {}
};


template<typename T>
class ObjectInitializerSelector {
public:
    using Type = std::conditional_t<
        std::is_base_of_v<Object, T>,
        ObjectInitializer,
        DumbInitializer
    >;
};

}