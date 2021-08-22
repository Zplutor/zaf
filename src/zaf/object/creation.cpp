#include <zaf/object/creation.h>
#include <zaf/application.h>
#include <zaf/object/object_type.h>
#include <zaf/object/internal/reflection_manager.h>

namespace zaf {
namespace internal {

std::shared_ptr<Object> CreateReflectionObjectByName(const std::wstring& name) {

    auto type = zaf::internal::ReflectionManager::Instance().GetType(name);
    if (type) {
        return type->CreateInstance();
    }
    return {};
}

}
}