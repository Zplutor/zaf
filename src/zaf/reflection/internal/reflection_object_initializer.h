#pragma once

namespace zaf {

class ReflectionObject;

namespace internal {

class ReflectionObjectInitializer {
public:
    static void Initialize(ReflectionObject& object);

public:
    ReflectionObjectInitializer() = delete;
};

}
}