#include <zaf/object/internal/object_initializer.h>
#include <zaf/object/object.h>

namespace zaf::internal {

void ObjectInitializer::Initialize(Object* object) {
    object->InitializeObject();
}

}