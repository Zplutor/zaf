#pragma once

namespace zaf {

namespace dynamic {
class Object;
}

namespace internal {

class ObjectInitializer {
public:
    static void Initialize(dynamic::Object* object);

public:
    ObjectInitializer() = delete;
};

}
}