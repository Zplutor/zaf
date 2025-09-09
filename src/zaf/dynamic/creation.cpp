#include <zaf/dynamic/creation.h>
#include <zaf/dynamic/object_type.h>
#include <zaf/dynamic/internal/reflection_manager.h>

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