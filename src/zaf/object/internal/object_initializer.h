#pragma once

namespace zaf {

class Object;

namespace internal {

class ObjectInitializer {
public:
    static void Initialize(Object& object);

public:
    ObjectInitializer() = delete;
};

}
}