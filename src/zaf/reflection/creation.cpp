#include <zaf/reflection/creation.h>
#include <zaf/application.h>
#include <zaf/object/object_type.h>
#include <zaf/reflection/reflection_manager.h>

namespace zaf {
namespace internal {

std::shared_ptr<Object> CreateReflectionObjectByName(const std::wstring& name) {

    auto type = GetReflectionManager().GetType(name);
    if (type) {
        return type->CreateInstance();
    }
    return {};
}

}
}