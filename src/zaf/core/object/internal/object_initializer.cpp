#include <zaf/core/object/internal/object_initializer.h>
#include <zaf/core/object/object.h>

namespace zaf::internal {

void ObjectInitializer::Initialize(Object* object) {
    object->InitializeObject();
}

}