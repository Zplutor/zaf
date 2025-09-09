#include <zaf/dynamic/internal/object_initializer.h>
#include <zaf/dynamic/object.h>

namespace zaf::internal {

void ObjectInitializer::Initialize(dynamic::Object* object) {
    object->InitializeObject();
}

}