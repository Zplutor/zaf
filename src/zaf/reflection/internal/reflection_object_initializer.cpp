#include <zaf/reflection/internal/reflection_object_initializer.h>
#include <zaf/reflection/reflection_object.h>

namespace zaf::internal {

void ReflectionObjectInitializer::Initialize(ReflectionObject& object) {
    object.InitializeObject();
}

}